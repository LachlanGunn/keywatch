#ifndef KEYWATCH_DAEMON_WORKER_H_
#define KEYWATCH_DAEMON_WORKER_H_

#include "keys/keys.h"
#include "daemon/config.h"

namespace keywatch {
namespace daemon {

void workerThread(Recipient recipient,
                  boost::lockfree::queue<
                  std::list<keywatch::keys::PublicKey>* >* responses);

}
}

#endif  // KEYWATCH_DAEMON_WORKER_H_
