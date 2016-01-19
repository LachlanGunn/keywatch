// Copyright 2016 Lachlan Gunn

#ifndef KEYWATCHD_HKP_PARSER_H_
#define KEYWATCHD_HKP_PARSER_H_

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
  int parserState;
};

  } // namespace hkp
} // namespace keywatch

#endif  // KEYWATCHD_HKP_PARSER_H
