// Copyright 2016 Lachlan Gunn

#include <stdio.h>

#include <cstdlib>
#include <string>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <deque>

#include <boost/program_options.hpp> // NOLINT

#include "hkp/hkp.h"
#include "keys/keys.h"
#include "daemon/worker.h"

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

namespace po = boost::program_options;

[[noreturn]]
int main(int argc, char** argv) {
  keywatch::hkp::HKPInit();

  try {
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
      std::exit(1);
    }

    std::cerr << "Preparing to do lookup.\n";

    keywatch::hkp::HKPServer server("http://jirk5u4osbsr34t5.onion:11371",
                                    "localhost:9050");

    std::vector<std::string> recipients =
        vm["recipient"].as< std::vector<std::string> >();

    std::list< std::unique_ptr<std::thread> > threads;
    std::queue<PublicKey> responses;
    std::mutex queue_mutex;
    std::condition_variable queue_condition_variable;

    for (std::vector<std::string>::const_iterator i = recipients.begin();
         i != recipients.end(); i++) {
      std::cerr << "Looking up " << *i << "\n";

      threads.push_back(std::unique_ptr<std::thread>(
          new std::thread(std::bind(keywatch::daemon::workerThread,
                                    keywatch::daemon::Recipient(*i),
                                    std::ref(queue_mutex),
                                    std::ref(queue_condition_variable),
                                    &responses))));

    }

    while (true) {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      if(responses.empty()) {
        queue_condition_variable.wait(queue_lock);
      }

      PublicKey key = responses.front();
      responses.pop();
      queue_lock.unlock();

      printf("Key: %s\n", key.identifier().c_str());

      std::list<UserID>::const_iterator iterator_uid, end_uid;
      std::list<UserID> uids = key.uids();
      for (iterator_uid = uids.begin(), end_uid = uids.end();
           iterator_uid != end_uid;
           iterator_uid++) {
        printf("    UID: %s\n", iterator_uid->identifier().c_str());
      }
    }
  }
  catch (...) {
    keywatch::hkp::HKPCleanup();
    throw;
  }
}
