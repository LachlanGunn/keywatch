// Copyright (c) 2016 Lachlan Gunn
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

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

#ifndef KEYWATCH_DEFAULT_KEYSERVER
#define KEYWATCH_DEFAULT_KEYSERVER "http://jirk5u4osbsr34t5.onion:11371"
#endif

#ifndef KEYWATCH_DEFAULT_PROXY
#define KEYWATCH_DEFAULT_PROXY "localhost:9050"
#endif

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

namespace po = boost::program_options;

int main(int argc, char** argv) {
  keywatch::hkp::HKPInit();

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "    Produce this help message.")
        ("recipient,r", po::value< std::vector<std::string> >(),
         "    Email addresses to look up.")
        ("keyserver,s", po::value< std::string >()
         ->default_value(KEYWATCH_DEFAULT_KEYSERVER),
         "    The keyserver to audit.")
        ("proxy,p", po::value< std::string >()
         ->default_value(KEYWATCH_DEFAULT_PROXY),
         "    The proxy by which to connect to the keyserver.");

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

    if (vm.count("recipient") == 0) {
      std::cerr << "No email addresses specified." << std::endl;
      return 1;
    }

    std::string keyserver = vm["keyserver"].as<std::string>();
    std::string proxy = vm["proxy"].as<std::string>();

    std::vector<std::string> recipients;
    recipients = vm["recipient"].as< std::vector<std::string> >();

    std::list< std::unique_ptr<std::thread> > threads;
    std::queue<PublicKey> responses;
    std::mutex queue_mutex;
    std::condition_variable queue_condition_variable;

    for (std::vector<std::string>::const_iterator i = recipients.begin();
         i != recipients.end(); i++) {

      threads.push_back(std::unique_ptr<std::thread>(
          new std::thread(std::bind(keywatch::daemon::workerThread,
                                    keywatch::daemon::Recipient(*i, keyserver,
                                                                proxy),
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

  return 0;
}
