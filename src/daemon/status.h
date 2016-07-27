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

#ifndef KEYWATCH_DAEMON_STATUS_H_
#define KEYWATCH_DAEMON_STATUS_H_

#include <mutex>

#include "daemon/config.h"

namespace keywatch {
namespace daemon {

/**
 * Status information for a given key.  Allows us to maintain the
 * state necessary for meaningful displays.
 */
class KeyStatus {
 public:
  explicit KeyStatus(const Recipient& recipient);

  Recipient recipient();
  
  int64_t request_count();
  int64_t mismatch_count();
  int64_t error_count();
  int64_t current_streak();

  /**
   * Add one response of the expected value to the count.
   */
  void RegisterMatch();

  /**
   * Add one response of an unexpected value to the count.
   */
  void RegisterMismatch();

  /**
   * Add one failed request to the count.
   */
  void RegisterError();

  /**
   * Reset status statistics.  This returns the object to its initial state.
   */
  void Reset();
  
 protected:
  std::mutex status_mutex;
 
 private:
  Recipient recipient_;
  int64_t successes_;
  int64_t failures_;
  int64_t errors_;
  int64_t streak_;
};

} //   namespace daemon
} // namespace keywatch

#endif
