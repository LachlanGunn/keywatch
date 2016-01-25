// Copyright 2016 Lachlan Gunn

#include <stdio.h>

#include <string>
#include <iostream>

#include <boost/program_options.hpp> // NOLINT

#include "hkp/hkp.h"
#include "keys/keys.h"

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

namespace po = boost::program_options;

int main(int argc, char** argv) {
  keywatch::hkp::HKPInit();

  po::options_description desc("Allozed options");
  desc.add_options()
      ("help", "produce help message")
      ("recipient,r", po::value< std::vector<std::string> >(),
                      "recipient to look up");

  po::positional_options_description p;
  p.add("recipient", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
            .options(desc)
            .positional(p)
            .run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  std::cerr << "Preparing to do lookup.\n";

  keywatch::hkp::HKPServer server("http://jirk5u4osbsr34t5.onion:11371",
                                  "localhost:9050");

  std::vector<std::string> recipients =
      vm["recipient"].as< std::vector<std::string> >();

  for (std::vector<std::string>::const_iterator i = recipients.begin();
       i != recipients.end(); i++) {
    std::cerr << "Looking up " << *i << "\n";

    std::list<PublicKey> keys = server.GetKeys(*i);

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
  }

  keywatch::hkp::HKPCleanup();
  return 0;
}
