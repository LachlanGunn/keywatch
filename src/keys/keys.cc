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

#include <stdint.h>

#include "keys/keys.h"

namespace keywatch {
namespace keys {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

const std::string& PublicKey::identifier() const {
  return _identifier;
}

void PublicKey::setIdentifier(const std::string& identifier) {
  _identifier = identifier;
}

const int32_t& PublicKey::keyLength() const {
  return _key_length;
}

void PublicKey::setKeyLength(const int32_t& keyLength) {
  _key_length = keyLength;
}

const int64_t& PublicKey::creationTime() const {
  return _creation_time;
}

void PublicKey::setCreationTime(const int64_t& creationTime) {
  _creation_time = creationTime;
}

const int64_t& PublicKey::expirationTime() const {
  return _expiration_time;
}

void PublicKey::setExpirationTime(const int64_t& expirationTime) {
  _expiration_time = expirationTime;
}

const std::string& PublicKey::flags() const {
  return _flags;
}

const std::list<UserID>& PublicKey::uids() const {
  return _uids;
}

void PublicKey::addUid(UserID uid) {
  _uids.push_back(uid);
}

const std::string& UserID::identifier() const {
  return _identifier;
}

void UserID::setIdentifier(const std::string& identifier) {
  _identifier = identifier;
}

const int64_t& UserID::creationTime() const {
  return _creation_time;
}

void UserID::setCreationTime(const int64_t& creationTime) {
  _creation_time = creationTime;
}

const int64_t& UserID::expirationTime() const {
  return _expiration_time;
}

void UserID::setExpirationTime(const int64_t& expirationTime) {
  _expiration_time = expirationTime;
}

const std::string& UserID::flags() const {
  return _flags;
}

}    // namespace keys
}  // namespace keywatch
