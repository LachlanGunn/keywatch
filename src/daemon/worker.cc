// Copyright (c) 2016 Lachlan Gunn
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <ctime>
#include <cstdint>
#include <cmath>
#include <queue>
#include <algorithm>
#include <memory>

#include <cryptopp/osrng.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "daemon/worker.h"
#include "daemon/config.h"
#include "hkp/hkp.h"
#include "keys/keys.h"

namespace keywatch {
namespace daemon {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

static double GetNextOffset() {
  uint64_t offset_int;
  CryptoPP::NonblockingRng rng;
  rng.GenerateBlock(reinterpret_cast<byte*>(&offset_int), sizeof(offset_int));
  return ((double)offset_int)/UINT64_MAX;
}

static const std::chrono::system_clock::time_point GetEpoch() {
  // Set our epoch at 2000-01-01T0000Z
  struct std::tm epoch_date = { 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0 };
  std::time_t epoch_time_t = timegm(&epoch_date);
  return std::chrono::system_clock::from_time_t(epoch_time_t);
}

template <class duration>
static double GetPeriodNumberForTimePoint(
    std::chrono::milliseconds tick_duration,
    std::chrono::time_point<std::chrono::system_clock, duration> time_point) {
  auto time_since_epoch = time_point - GetEpoch();

  return (double)(time_since_epoch / tick_duration);
}

static std::chrono::time_point<std::chrono::system_clock,
                               std::chrono::milliseconds>
GetCurrentPeriodEnd(std::chrono::milliseconds tick_duration) {
  using std::chrono::system_clock;
  auto epoch = GetEpoch();
  double period_number = GetPeriodNumberForTimePoint(tick_duration,
                                                     system_clock::now());
  return std::chrono::time_point_cast<std::chrono::milliseconds>(epoch + tick_duration*std::ceil(period_number));
}

static void WaitForNextRequest(
    std::chrono::milliseconds tick_length,
    std::chrono::system_clock::time_point window,
    std::mutex& exit_mutex,
    std::condition_variable& exit_condition_variable) {  
  double offset = GetNextOffset();
  auto next_tick = window + tick_length*offset;

  std::unique_lock<std::mutex> lock(exit_mutex);
  exit_condition_variable.wait_until(lock, next_tick);
}

/**
 * Determine whether a set of HKP response flags indicate an active entry.
 *
 * AreFlagsActive checks a flags string from an HKP response to determine
 * whether the corresponding entry is active.  In particular, this means
 * checking whether it contains the characters 'r', 'e', or 'd'.
 *
 * \param[in]  flags    The flag string to check.
 *
 * return  A boolean value that is true if and only if the entry is active.
 */
static bool AreFlagsActive(std::string flags) {
  return flags.end() == std::find_if(flags.begin(), flags.end(),
                                     [](char c) -> bool {
                                       switch (c) {
                                         case 'r':
                                         case 'e':
                                         case 'd':
                                           return true;
                                         default:
                                           return false;
                                       };
                                     });
}


/**
 * Determine whether a key is valid for the given recipient.
 *
 * The IsValidKey function determines whether or not a PublicKey object
 * is could conceivably be used to communicate with the recipient.  In
 * particular, it checks that both of the following are true:
 *
 *   1. At least one UID contains a string of the form "<email>", where
 *        <email> is the recipient's email address.
 *   2. The key does not have its "expired", "revoked", or "disabled"
 *        flags set.
 *
 * \param[in] recipient    The recipient against whom the public key should
 *                           be validated.
 * \param[in] key          The key which is to be checked.
 *
 * \return A boolean value---true if and only if both conditions are met.
 */
static bool IsValidKey(const Recipient& recipient, const PublicKey& key) {
  std::list<UserID> uids = key.uids();
  // Find the first UserID uid such that uid.identifier() contains "<email>".
  std::list<UserID>::iterator matching_uid
      = std::find_if(uids.begin(), uids.end(),
                     [&recipient](UserID uid) -> bool {
                       if (uid.identifier().find(
                               (boost::format("<%1%>") % recipient.email())
                               .str()) == std::string::npos) {
                         return false;
                       }
                       return AreFlagsActive(uid.flags());
                     });
  // We couldn't find a UID matching the email.
  if (matching_uid == uids.end()) {
    return false;
  }

  // Now check that the key hasn't expired/been revoked/been disabled.
  return AreFlagsActive(key.flags());
}

static std::unique_ptr<const PublicKey> FindFirstKey(
    Recipient recipient,
    std::list<PublicKey> keys) {
  std::list<PublicKey>::const_iterator first_match
      = std::find_if(keys.begin(), keys.end(),
                     [&recipient](const PublicKey& key) -> bool {
                       return IsValidKey(recipient, key);
                     });

  return (first_match == keys.end())
      ? nullptr
      : std::unique_ptr<const PublicKey>(
            new PublicKey(*first_match));
}

/**
 * Worker thread: download the keys.
 */
void workerThread(Recipient recipient,
                  std::mutex& queue_mutex,
                  std::condition_variable& queue_condition_variable,
                  bool& finished,
                  std::mutex& exit_mutex,
                  std::condition_variable& exit_condition_variable,
                  std::shared_ptr<KeyStatus> status,
                  std::queue<PublicKey>* responses) {
  keywatch::hkp::HKPServer server(recipient.keyserver(), recipient.proxy());

  std::string email = recipient.email();
  std::unique_ptr<std::string> fingerprint(nullptr);

  auto period = std::chrono::seconds(10);
  auto next_period_start = GetCurrentPeriodEnd(period);

  while (!finished) {
    WaitForNextRequest(period, next_period_start,
                       exit_mutex, exit_condition_variable);
    next_period_start += period;

    // If shutdown has started then we should escape before the request
    // has gone through.
    if (finished) {
      break;
    }

    std::list<PublicKey> keys = server.GetKeys(email);
    std::unique_ptr<const PublicKey> first_key = FindFirstKey(recipient, keys);
    if (nullptr == first_key) {
      //std::cerr << "No valid key returned." << std::endl;
      status->RegisterError();
      continue;
    }

    if (nullptr == fingerprint) {
      fingerprint.reset(new std::string(first_key->identifier()));
    }
    else if(first_key->identifier() != *fingerprint) {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      responses->push(*first_key);
      queue_lock.unlock();
      queue_condition_variable.notify_one();

      // Update the key statistics.
      status->RegisterMismatch();
    }
    else {
      status->RegisterMatch(first_key->identifier());
    }

    *fingerprint = first_key->identifier();
  }
}

} // namespace daemon
} // namespace keywatch
