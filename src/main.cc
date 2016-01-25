// Copyright 2016 Lachlan Gunn

#include <stdio.h>

#include <string>
#include <iostream>
#include <functional>
#include <thread>
#include <memory>

#include <boost/program_options.hpp> // NOLINT

#include "hkp/hkp.h"
#include "keys/keys.h"
#include "daemon/worker.h"

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

  std::list< std::unique_ptr<std::thread> > threads;
 
  for (std::vector<std::string>::const_iterator i = recipients.begin();
       i != recipients.end(); i++) {
    std::cerr << "Looking up " << *i << "\n";

    threads.push_back(std::unique_ptr<std::thread>(
        new std::thread(std::bind(keywatch::daemon::workerThread,
                                  keywatch::daemon::Recipient(*i)))));

  }

  for(std::list< std::unique_ptr<std::thread> >::const_iterator current_thread
          = threads.begin();
      current_thread != threads.end(); current_thread++) {
    (*current_thread)->join();
  }

  keywatch::hkp::HKPCleanup();
  return 0;
}
