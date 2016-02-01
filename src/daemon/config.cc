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

#include "daemon/config.h"

#include <list>
#include <string>

namespace keywatch {
namespace daemon {

Recipient::Recipient(const std::string& email, const std::string& keyserver,
                     const std::string& proxy)
    : email_(email), keyserver_(keyserver), proxy_(proxy) {}

const std::string& Recipient::email() const {
  return email_;
}

void Recipient::setEmail(const std::string& email) {
  email_ = email;
}

const std::string& Recipient::keyserver() const {
  return keyserver_;
}

void Recipient::setKeyserver(const std::string& keyserver) {
  keyserver_ = keyserver;
}

const std::string& Recipient::proxy() const {
  return proxy_;
}

void Recipient::setProxy(const std::string& proxy) {
  proxy_ = proxy;
}

Configuration::Configuration() {}

const std::list<Recipient>& Configuration::recipients() const {
  return recipients_;
}

} // namespace daemon
} // namespace daemon
