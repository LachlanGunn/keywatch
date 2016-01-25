#include <thread>
#include <list>

#include <cryptopp/osrng.h>

#include "daemon/config.h"
#include "hkp/hkp.h"
#include "keys/keys.h"

namespace keywatch {
namespace daemon {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

void workerThread(Recipient recipient) {
  keywatch::hkp::HKPServer server("http://jirk5u4osbsr34t5.onion:11371",
                                  "localhost:9050");

  std::string email = recipient.email();

  while (true) {
    std::list<PublicKey> keys = server.GetKeys(email);

    std::list<PublicKey>::const_iterator iterator, end;
    for (iterator = keys.begin(), end = keys.end();
         iterator != end;
         iterator++) {
      printf("Key: %s\n", (*iterator).identifier().c_str());

      std::list<UserID>::const_iterator iterator_uid, end_uid;
      std::list<UserID> uids = iterator->uids();
      for (iterator_uid = uids.begin(), end_uid = uids.end();
           iterator_uid != end_uid;
           iterator_uid++) {
        printf("    UID: %s\n", iterator_uid->identifier().c_str());
      }
    }

    break;
  }
}

} // namespace daemon
} // namespace keywatch
