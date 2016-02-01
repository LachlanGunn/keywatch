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

#ifndef KEYWATCH_KEYS_KEYS_H_
#define KEYWATCH_KEYS_KEYS_H_

#include <stdint.h>

#include <string>
#include <list>

namespace keywatch {
  namespace keys {
  
class PublicKey;
class UserID;

class UserID {
 public:
  UserID(std::string  identifier = "",
       int64_t creation_time = 0,
       int64_t expiration_time = 0,
       std::string  flags = "")
    : _identifier(identifier),
      _creation_time(creation_time),
      _expiration_time(expiration_time),
      _flags(flags) {}

  const std::string& identifier() const;
  void               setIdentifier(const std::string& identifier);

  const int64_t& creationTime() const;
  void setCreationTime(const int64_t& creationTime);

  const int64_t& expirationTime() const;
  void setExpirationTime(const int64_t& expirationTime);

  const std::string& flags() const;
  void setFlags(const std::string& flags);

 private:
  std::string _identifier;
  int64_t _creation_time;
  int64_t _expiration_time;
  std::string _flags;
};

class PublicKey {
 public:
  PublicKey(std::string  identifier = "",     int32_t      algorithm  = 0,
            int32_t      key_length = 0,      int64_t      creation_time = 0,
            int64_t      expiration_time = 0, std::string  flags = "",
          std::list<UserID> uids = std::list<UserID>())
    : _identifier(identifier),
      _algorithm(algorithm),
      _key_length(key_length),
      _creation_time(creation_time),
      _expiration_time(expiration_time),
      _flags(flags),
      _uids(uids) {}


  const std::string& identifier() const;
  void  setIdentifier(const std::string& identifier);

  const int32_t& algorithm();
  void  setAlgorithm(const int32_t& identifier);

  const int32_t& keyLength() const;
  void  setKeyLength(const int32_t& keyLength);

  const int64_t& creationTime() const;
  void setCreationTime(const int64_t& creationTime);

  const int64_t& expirationTime() const;
  void setExpirationTime(const int64_t& expirationTime);

  const std::string& flags() const;
  void setFlags(const std::string& flags);

  const std::list<UserID>& uids() const;
  void addUid(UserID uid);

 private:
  std::string _identifier;
  int32_t _algorithm;
  int32_t _key_length;
  int64_t _creation_time;
  int64_t _expiration_time;
  std::string _flags;
  std::list<UserID> _uids;
};

  } // namespace keys
} // namespace keywatch

#endif  // KEYWATCH_KEYS_KEYS_H_
