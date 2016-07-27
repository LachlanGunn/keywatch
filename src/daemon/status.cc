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

#include <mutex>

#include "daemon/config.h"
#include "daemon/status.h"
#include "keys/keys.h"

namespace keywatch {
namespace daemon {

KeyStatus::KeyStatus(const Recipient& recipient)
    : recipient_(recipient) {
  Reset();
}

void KeyStatus::Reset() {
  std::lock_guard<std::mutex> guard(status_mutex);
  
  successes_ = 0;
  failures_ = 0;
  errors_ = 0;
  streak_ = 0;
}

void KeyStatus::RegisterMatch() {
  std::lock_guard<std::mutex> guard(status_mutex);
  
  successes_++;
  streak_++;
}

void KeyStatus::RegisterMismatch() {
  std::lock_guard<std::mutex> guard(status_mutex);
  
  failures_++;
  streak_ = 0;
}

void KeyStatus::RegisterError() {
  std::lock_guard<std::mutex> guard(status_mutex);
  
  errors_++;
}

Recipient KeyStatus::recipient() {
  // We don't need a lock here because recipient is immutable.
  return recipient_;
}

int64_t KeyStatus::request_count() {
  std::lock_guard<std::mutex> guard(status_mutex);
  return successes_ + failures_;
}

int64_t KeyStatus::mismatch_count() {
  std::lock_guard<std::mutex> guard(status_mutex);
  return failures_;
}

int64_t KeyStatus::error_count() {
  std::lock_guard<std::mutex> guard(status_mutex);
  return errors_;
}

int64_t KeyStatus::current_streak() {
  std::lock_guard<std::mutex> guard(status_mutex);
  return streak_;
}

} // namespace keywatch
} //   namespace daemon
