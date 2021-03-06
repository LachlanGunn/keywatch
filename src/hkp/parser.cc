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

#include "parser.h"

#include <stdint.h>

#include <string>
#include <tuple>
#include <algorithm>

#include <boost/spirit/include/qi.hpp>                // NOLINT
#include <boost/spirit/include/phoenix_core.hpp>      // NOLINT
#include <boost/spirit/include/phoenix_operator.hpp>  // NOLINT
#include <boost/spirit/include/phoenix_object.hpp>    // NOLINT

#include <boost/fusion/adapted/struct.hpp>            // NOLINT
#include <boost/fusion/include/io.hpp>                // NOLINT

#include <boost/optional.hpp>                         // NOLINT

#include "keys/keys.h"

namespace keywatch {
namespace hkp {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

using qi::int_;
using qi::lit;
using ascii::char_;

enum ParserState {
  AwaitingInfo = 0,
  AwaitingPub  = 1,
  AwaitingUid  = 2,
  Finished     = 3
};

struct raw_info {
  int64_t version;
  boost::optional<int64_t> count;
};

struct raw_uid {
  std::string user;
  boost::optional<int64_t> creation_date;
  boost::optional<int64_t> expiration_date;
  boost::optional<std::string> flags;
};

struct raw_public_key {
  std::string key_id;
  boost::optional<int32_t> algorithm;
  boost::optional<int32_t> length;
  boost::optional<int64_t> creation_date;
  boost::optional<int64_t> expiration_date;
  boost::optional<std::string> flags;
};

  }
}

BOOST_FUSION_ADAPT_STRUCT(
    keywatch::hkp::raw_info,
    (int64_t, version)
    (boost::optional<int64_t>, count)
)

BOOST_FUSION_ADAPT_STRUCT(
    keywatch::hkp::raw_uid,
    (std::string, user)
    (boost::optional<int64_t>, creation_date)
    (boost::optional<int64_t>, expiration_date)
    (boost::optional<std::string>, flags)
)

BOOST_FUSION_ADAPT_STRUCT(
    keywatch::hkp::raw_public_key,
    (std::string, key_id)
    (boost::optional<int32_t>, algorithm)
    (boost::optional<int32_t>, length)
    (boost::optional<int64_t>, creation_date)
    (boost::optional<int64_t>, expiration_date)
    (boost::optional<std::string>, flags)
)

namespace keywatch {
  namespace hkp {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;


template <typename Iterator>
struct info_parser : qi::grammar<Iterator, raw_info()> {
  info_parser() : info_parser::base_type(start) {
    using qi::int_;
    using qi::lit;
    using qi::lexeme;
    using ascii::char_;

    start %=
        lit("info")
        >> ':' >> -int_
        >> ':' >> -int_;
  }

  qi::rule<Iterator, raw_info()> start;
};

template <typename Iterator>
struct uid_parser : qi::grammar<Iterator, raw_uid()> {
  uid_parser() : uid_parser::base_type(start) {
    using qi::int_;
    using qi::lit;
    using qi::lexeme;
    using ascii::char_;

    start %=
        lit("uid")
        >> ':' >> *(char_ - ':')
        >> ':' >> -int_
        >> ':' >> -int_
        >> ':' >> *(char_ - ':');
  }

  qi::rule<Iterator, raw_uid()> start;
};

template <typename Iterator>
struct public_key_parser : qi::grammar<Iterator, raw_public_key()> {
  public_key_parser() : public_key_parser::base_type(start) {
    using qi::int_;
    using qi::lit;
    using ascii::char_;

    using boost::spirit::_1;
    using boost::spirit::_val;

    start %=
        lit("pub")
        >> ':' >> *(char_ - ':')
        >> ':' >> -int_
        >> ':' >> -int_
        >> ':' >> -int_
        >> ':' >> -int_
        >> ':' >> *(char_ - ':');
  }

  qi::rule<Iterator, raw_public_key()> start;
};

bool parse_info(std::string line, struct raw_info* result) {
  std::size_t separators_found = std::count(line.begin(), line.end(), ':');
  for (; separators_found < 2; separators_found++) {
    line += ':';
  }

  std::string::const_iterator first = line.begin();
  std::string::const_iterator last  = line.end();

  info_parser<std::string::const_iterator> grammar;
  bool success = qi::parse(first, last, grammar, *result);
  return success && (first == last);
}

bool parse_public_key(std::string line, struct raw_public_key* result) {
  std::size_t separators_found = std::count(line.begin(), line.end(), ':');
  for (; separators_found < 6; separators_found++) {
    line += ':';
  }

  std::string::const_iterator first = line.begin();
  std::string::const_iterator last  = line.end();

  public_key_parser<std::string::const_iterator> grammar;
  bool success = qi::parse(first, last, grammar, *result);
  return success && (first == last);
}

bool parse_uid(std::string line, struct raw_uid* result) {
  std::size_t separators_found = std::count(line.begin(), line.end(), ':');
  for (; separators_found < 4; separators_found++) {
    line += ':';
  }

  std::string::const_iterator first = line.begin();
  std::string::const_iterator last  = line.end();

  uid_parser<std::string::const_iterator> grammar;
  bool success = qi::parse(first, last, grammar, *result);
  return success && (first == last);
}

static std::tuple<bool, std::string, std::string> find_newline(
    std::string str) {
  std::size_t index;
  std::size_t firstIndex = std::string::npos;
  std::size_t newlineLength = 0;

  index = str.find("\r\n");
  if (index != std::string::npos) {
    newlineLength = 2;
    firstIndex = index;
  }

  index = str.find("\n");
  if (index != std::string::npos && index < firstIndex) {
    newlineLength = 1;
    firstIndex = index;
  }

  index = str.find("\r");
  if (index != std::string::npos && index < firstIndex) {
    newlineLength = 1;
    firstIndex = index;
  }

  if (firstIndex != std::string::npos) {
    return std::tuple<bool, std::string, std::string>(
        true,
        str.substr(0, firstIndex),
        str.substr(firstIndex+newlineLength));
  } else {
    return std::tuple<bool, std::string, std::string>(false, std::string(),
                                                        str);
  }
}

HKPResponseParser::HKPResponseParser() {
  parserState = AwaitingInfo;
}

const std::list<PublicKey>& HKPResponseParser::keys() const {
  return _keys;
}

void HKPResponseParser::parseCharacters(std::string newText) {
  std::string currentLine;
  bool newLineToProcess;

  buffer += newText;

  while (1) {
    std::tuple<bool&, std::string&, std::string&>(newLineToProcess,
                                                    currentLine,
                                                    buffer)
        = find_newline(buffer);

    if (!newLineToProcess) {
      break;
    } else {
      parseLine(currentLine);
    }
  }
}

void HKPResponseParser::parseLine(std::string line) {
  if (line.size() == 0) {
    return;
  }

  struct raw_info info;
  struct raw_uid uid;
  struct raw_public_key pub;

  switch (parserState) {
    case AwaitingInfo:
      if (parse_info(line, &info)) {
        parserState = AwaitingPub;
      }
      break;
    case AwaitingPub:
      if (parse_public_key(line, &pub)) {
        _keys.push_back(PublicKey(
            pub.key_id,
            pub.algorithm ? *pub.algorithm : -1,
            pub.length ? *pub.length : -1,
            pub.creation_date ? *pub.creation_date : -1,
            pub.expiration_date ? *pub.expiration_date : -1,
            pub.flags ? *pub.flags : ""));

        parserState = AwaitingUid;
      }
      break;
    case AwaitingUid:
      if (parse_uid(line, &uid)) {
        _keys.back().addUid(UserID(
            uid.user,
            uid.creation_date ? *uid.creation_date : -1,
            uid.expiration_date ? *uid.expiration_date : -1,
            uid.flags ? *uid.flags : ""));
      } else if (parse_public_key(line, &pub)) {
        _keys.push_back(PublicKey(
            pub.key_id,
            pub.algorithm ? *pub.algorithm : -1,
            pub.length ? *pub.length : -1,
            pub.creation_date ? *pub.creation_date : -1,
            pub.expiration_date ? *pub.expiration_date : -1,
            pub.flags ? *pub.flags : ""));
      }
  }
}

void HKPResponseParser::flush() {
  parseLine(buffer);
  buffer = std::string();
}

} // namespace hkp
} // namespace keywatch
