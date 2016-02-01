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

#ifndef KEYWATCH_HKP_HKP_H_
#define KEYWATCH_HKP_HKP_H_

#include <stdint.h>

#include <string>
#include <list>
#include <memory>

#include "keys/keys.h"

namespace keywatch {
namespace hkp {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

extern bool HKPInit();
extern void HKPCleanup();

class HKPServer {
 public:
  explicit HKPServer(std::string host, std::string proxy = "");
  ~HKPServer();

  const std::list<PublicKey> GetKeys(std::string email);

  const std::string& host() const;
  void setHost(const std::string& host);

  const std::string& proxy() const;
  void setProxy(const std::string& proxy);

 private:
  struct HKPServerState_;

  HKPServer(HKPServer& rhs);
  HKPServer& operator=(HKPServer& rhs);

  std::string host_;
  std::string proxy_;
  std::unique_ptr<struct HKPServerState_> state_;
};

}  //   namespace hkp
}  // namespace keywatch

#endif  // KEYWATCH_HKP_HKP_H_
