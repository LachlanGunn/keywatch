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

static void WaitForNextRequest(double tick_length) {
  // Set our epoch at 2000-01-01T0000Z
  struct std::tm epoch_date { 0, 0, 0, 0, 0, 100, 0, 0, 0 };
  std::time_t epoch_time_t = timegm(&epoch_date);
  std::chrono::system_clock::time_point epoch_point
      = std::chrono::system_clock::from_time_t(epoch_time_t);

  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

  std::chrono::duration<double> tick_duration(tick_length);
  double current_tick_number = (now - epoch_point) / tick_duration;
  double ticks_remaining = std::ceil(current_tick_number) - current_tick_number;
  std::chrono::duration<int> time_remaining =
      std::chrono::duration_cast<std::chrono::duration<int>>(
          tick_duration*(ticks_remaining + GetNextOffset()*0.8));
  
  std::chrono::system_clock::time_point next_tick
      = now + time_remaining;
  std::this_thread::sleep_until(next_tick);
}

void workerThread(Recipient recipient,
                  std::mutex& queue_mutex,
                  std::condition_variable& queue_condition_variable,
                  std::queue<PublicKey>* responses) {
  keywatch::hkp::HKPServer server("http://jirk5u4osbsr34t5.onion:11371",
                                  "localhost:9050");
  std::string email = recipient.email();
  std::unique_ptr<std::string> fingerprint(nullptr);

  while (true) {
    WaitForNextRequest(10.0);
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
