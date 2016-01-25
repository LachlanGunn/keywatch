#ifndef KEYWATCH_DAEMON_WORKER_H_
#define KEYWATCH_DAEMON_WORKER_H_

#include "daemon/config.h"

namespace keywatch {
namespace daemon {

void workerThread(Recipient recipient);

}
}

#endif  // KEYWATCH_DAEMON_WORKER_H_
