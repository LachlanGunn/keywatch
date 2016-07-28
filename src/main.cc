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
#include <exception>

#include <boost/program_options.hpp> // NOLINT
#include <ncurses.h>

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

  // We need these during cleanup, so we declare them up here.
  std::list< std::unique_ptr<std::thread> > threads;
  bool finished;
  std::mutex exit_mutex;
  std::condition_variable exit_condition_variable;

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

    std::vector< std::shared_ptr<keywatch::daemon::KeyStatus> >
        recipient_statuses(recipients.size());

    std::queue<PublicKey> responses;
    std::mutex queue_mutex;
    std::condition_variable queue_condition_variable;

    size_t longest_email = 0;

    int thread_number = 0;
    for (std::vector<std::string>::const_iterator i = recipients.begin();
         i != recipients.end(); i++, thread_number++) {

      if( i->size() > longest_email ) {
        longest_email = i->size();
      }
      
      recipient_statuses[thread_number] =
          std::shared_ptr<keywatch::daemon::KeyStatus>(
              new keywatch::daemon::KeyStatus(
                  keywatch::daemon::Recipient(*i, keyserver, proxy)));

      threads.push_back(std::unique_ptr<std::thread>(
          new std::thread(std::bind(keywatch::daemon::workerThread,
                                    recipient_statuses[
                                        thread_number]->recipient(),
                                    std::ref(queue_mutex),
                                    std::ref(queue_condition_variable),
                                    std::ref(finished),
                                    std::ref(exit_mutex),
                                    std::ref(exit_condition_variable),
                                    recipient_statuses[thread_number],
                                    &responses))));
    }

    initscr();
    start_color();
    raw();
    noecho();
    halfdelay(1);

    // Set up display colours.
    //   0 => Normal
    //   1 => Green, for good keys
    //   2 => Red,   for bad keys
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED,   COLOR_BLACK);

    while (true) {
      attron(A_BOLD);
      mvprintw(0, longest_email, "Streak / Total / Error");
      attroff(A_BOLD);

      int j = 0;
      for (auto i = recipient_statuses.begin();
           i != recipient_statuses.end();
           i++, j++) {
        bool warning = (*i)->mismatch_count() > 0;
        if (warning) {
          attron(COLOR_PAIR(2));
        }
        else if( (*i)->current_streak() >= 10 ) {
          attron(COLOR_PAIR(1));
        }

        std::string email = (*i)->recipient().email();
        mvprintw(j+1, 0, "%s", email.c_str());
        attroff(COLOR_PAIR(2));
        attroff(COLOR_PAIR(1));

        mvprintw(j+1, 1+longest_email, "%5d / %5d / %5d", 
                 (*i)->current_streak(),
                 (*i)->request_count(),
                 (*i)->error_count());

        if (warning) {
          attron(COLOR_PAIR(2));
          printw(" Inconsistent!");
          attroff(COLOR_PAIR(2));
        }
      }
      refresh();
      int character_input = getch();
      if('q' == character_input) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
  catch (std::exception e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
  }

  finished = true;
  exit_condition_variable.notify_all();
  for (auto i = threads.begin(); i != threads.end(); i++) {
    (*i)->join();
  }
  endwin();
  keywatch::hkp::HKPCleanup();
  return 1;
}
