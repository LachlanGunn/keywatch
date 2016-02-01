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

#ifndef KEYWATCH_HKP_PARSER_H_
#define KEYWATCH_HKP_PARSER_H_

#include <cstdint>
#include <string>
#include <list>

#include "keys/keys.h"

namespace keywatch {
  namespace hkp {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;


class HKPResponseParser;

class HKPResponseParser {
 public:
  HKPResponseParser();

  void parseCharacters(std::string newText);
  void flush();

  const std::list<PublicKey>& keys() const;

 protected:
  void parseLine(std::string newLine);
  
 private:
  std::list<PublicKey> _keys;
  std::string buffer;
  uint64_t parserState;
};

  } // namespace hkp
} // namespace keywatch

#endif  // KEYWATCH_HKP_PARSER_H
