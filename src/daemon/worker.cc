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

#include <cryptopp/osrng.h>
#include <boost/date_time/posix_time/posix_time.hpp>

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

static void WaitForNextRequest(std::chrono::milliseconds tick_length,
                               std::chrono::system_clock::time_point window) {  
  double offset = GetNextOffset();
  auto next_tick = window + tick_length*offset;

  std::this_thread::sleep_until(next_tick);
}

void workerThread(Recipient recipient,
                  std::mutex& queue_mutex,
                  std::condition_variable& queue_condition_variable,
                  std::queue<PublicKey>* responses) {
  keywatch::hkp::HKPServer server(recipient.keyserver(), recipient.proxy());

  std::string email = recipient.email();
  std::unique_ptr<std::string> fingerprint(nullptr);

  auto period = std::chrono::seconds(10);
  auto next_period_start = GetCurrentPeriodEnd(period);

  while (true) {
    WaitForNextRequest(period, next_period_start);
    next_period_start += period;

    std::list<PublicKey> keys = server.GetKeys(email);
    if (keys.empty()) {
      continue;
    }

    PublicKey first_key = keys.front();

    if (nullptr == fingerprint) {
      fingerprint.reset(new std::string(first_key.identifier()));
    }
    else if(first_key.identifier() != *fingerprint) {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      responses->push(keys.front());
      queue_lock.unlock();
      queue_condition_variable.notify_one();
    }
  }
}

} // namespace daemon
} // namespace keywatch
