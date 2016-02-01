#ifndef KEYWATCH_DAEMON_WORKER_H_
#define KEYWATCH_DAEMON_WORKER_H_

#include <thread>
#include <mutex>
#include <condition_variable>

#include "keys/keys.h"
#include "daemon/config.h"

namespace keywatch {
namespace daemon {

[[noreturn]]
void workerThread(Recipient recipient,
                  std::mutex& queue_mutex,
                  std::condition_variable& queue_condition_variable,
                  std::queue<keywatch::keys::PublicKey>* responses);

}
}

#endif  // KEYWATCH_DAEMON_WORKER_H_
