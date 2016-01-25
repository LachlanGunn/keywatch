#include <thread>
#include <list>
#include <ctime>
#include <cstdint>
#include <cmath>

#include <cryptopp/osrng.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lockfree/queue.hpp> 

#include "daemon/config.h"
#include "hkp/hkp.h"
#include "keys/keys.h"

namespace keywatch {
namespace daemon {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

static void WaitForNextRequest(double tick_length, double offset) {
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
          tick_duration*ticks_remaining);
  
  std::chrono::system_clock::time_point next_tick
      = now + time_remaining;
  std::this_thread::sleep_until(next_tick);
}

void workerThread(Recipient recipient,
                  boost::lockfree::queue<std::list<PublicKey>* >* responses) {
  keywatch::hkp::HKPServer server("http://jirk5u4osbsr34t5.onion:11371",
                                  "localhost:9050");

  std::string email = recipient.email();

  while (true) {
    WaitForNextRequest(30.0, 0);
    std::list<PublicKey> keys = server.GetKeys(email);

    responses->push(new std::list<PublicKey>(keys));    
  }
}

} // namespace daemon
} // namespace keywatch
