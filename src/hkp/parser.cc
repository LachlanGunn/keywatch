// Copyright 2016 Lachlan Gunn

#include "parser.h"

#include <stdint.h>

#include <string>
#include <tuple>

#include <boost/spirit/include/qi.hpp>                // NOLINT
#include <boost/spirit/include/phoenix_core.hpp>      // NOLINT
#include <boost/spirit/include/phoenix_operator.hpp>  // NOLINT
#include <boost/spirit/include/phoenix_object.hpp>    // NOLINT

#include <boost/fusion/adapted/struct.hpp>            // NOLINT
#include <boost/fusion/include/io.hpp>                // NOLINT

#include <boost/config.hpp>                           // NOLINT

#include "keys/keys.h"

namespace keywatch {
  namespace hkp {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;


namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

enum ParserState {
  AwaitingInfo = 0,
  AwaitingPub  = 1,
  AwaitingUid  = 2,
  Finished     = 3
};

struct raw_info {
  int32_t version;
  int64_t count;
};

struct raw_uid {
  std::string user;
  int64_t creation_date;
  int64_t expiration_date;
  std::string flags;
};

struct raw_public_key {
  std::string key_id;
  int32_t algorithm;
  int32_t length;
  int64_t creation_date;
  int64_t expiration_date;
  std::string flags;
};

  }
}

BOOST_FUSION_ADAPT_STRUCT(
    keywatch::hkp::raw_info,
    (int32_t, version)
    (int64_t, count)
)

BOOST_FUSION_ADAPT_STRUCT(
    keywatch::hkp::raw_uid,
    (std::string, user)
    (int64_t, creation_date)
    (int64_t, expiration_date)
    (std::string, flags)
)

BOOST_FUSION_ADAPT_STRUCT(
    keywatch::hkp::raw_public_key,
    (std::string, key_id)
    (int32_t, algorithm)
    (int32_t, length)
    (int64_t, creation_date)
    (int64_t, expiration_date)
    (std::string, flags)
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
        >> -(':' >> -int_
        >> -(':' >> -int_));
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
        >> -(':' >> *(char_ - ':')
        >> -(':' >> -int_
        >> -(':' >> -int_
        >> -(':' >> *(char_ - ':')))));
  }

  qi::rule<Iterator, raw_uid()> start;
};

template <typename Iterator>
struct public_key_parser : qi::grammar<Iterator, raw_public_key()> {
  public_key_parser() : public_key_parser::base_type(start) {
    using qi::int_;
    using qi::lit;
    using qi::lexeme;
    using ascii::char_;

    start %=
        lit("pub")
        >> -(':' >> *(char_ - ':')
        >> -(':' >> -int_
        >> -(':' >> -int_
        >> -(':' >> -int_
        >> -(':' >> -int_
        >> -(':' >> *(char_ - ':')))))));
  }

  qi::rule<Iterator, raw_public_key()> start;
};

template <typename Iterator>
bool parse_info(Iterator first, Iterator last, struct raw_info* result) {
  info_parser<Iterator> grammar;
  return qi::parse(first, last, grammar, *result);
}

template <typename Iterator>
bool parse_public_key(Iterator first, Iterator last,
                        struct raw_public_key* result) {
  public_key_parser<Iterator> grammar;
  return qi::parse(first, last, grammar, *result);
}

template <typename Iterator>
bool parse_uid(Iterator first, Iterator last, struct raw_uid* result) {
  uid_parser<Iterator> grammar;
  return qi::parse(first, last, grammar, *result);
}

static std::tuple<bool, std::string, std::string> find_newline(
    std::string str) {
  std::size_t index;
  std::size_t firstIndex = std::string::npos;
  int newlineLength;

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
      if (parse_info(line.begin(), line.end(), &info)) {
        parserState = AwaitingPub;
      }


    case AwaitingPub:
      if (parse_public_key(line.begin(), line.end(), &pub)) {
        _keys.push_back(PublicKey(
            pub.key_id,
            pub.algorithm,
            pub.length,
            pub.creation_date,
            pub.expiration_date,
            pub.flags));

        parserState = AwaitingUid;
      }

      break;

    case AwaitingUid:
      if (parse_uid(line.begin(), line.end(), &uid)) {
        _keys.back().addUid(UserID(
            uid.user,
            uid.creation_date,
            uid.expiration_date,
            uid.flags));
      } else if (parse_public_key(line.begin(), line.end(), &pub)) {
        _keys.push_back(PublicKey(
            pub.key_id,
            pub.algorithm,
            pub.length,
            pub.creation_date,
            pub.expiration_date,
            pub.flags));
      }
  }
}

void HKPResponseParser::flush() {
  parseLine(buffer);
  buffer = std::string();
}

  } // namespace hkp
} // namespace keywatch
