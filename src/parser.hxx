#ifndef __PARSER_H
#define __PARSER_H

#include <string>
#include <list>

#include "keys.hxx"

class HKPResponseParser;

class HKPResponseParser
{
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

#endif
