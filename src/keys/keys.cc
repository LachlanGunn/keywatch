// Copyright 2016 Lachlan Gunn

#include <stdint.h>

#include "keys/keys.h"

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
