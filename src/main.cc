// Copyright 2016 Lachlan Gunn

#include <stdio.h>

#include <string>
#include <iostream>

#include "hkp/hkp.h"
#include "keys/keys.h"

int main(int argc, char** argv) {
  HKPInit();
  
  HKPServer server("http://jirk5u4osbsr34t5.onion:11371",
                   "localhost:9050");
  std::list<PublicKey> keys = server.GetKeys("lachlan@twopif.net");

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

  HKPCleanup();
  return 0;
}
