#ifndef __KEYS_HXX
#define __KEYS_HXX

#include <string>
#include <list>

class PublicKey;
class UserID;

class UserID
{
public:
    UserID(std::string  identifier = "",
	   unsigned int creation_time = 0,
	   unsigned int expiration_time = 0,
	   std::string  flags = "")
	: _identifier(identifier),
	  _creation_time(creation_time),
	  _expiration_time(expiration_time),
	  _flags(flags) {}

    const std::string& identifier() const;
    void               setIdentifier(const std::string& identifier);

    const unsigned long long int& creationTime() const;
    void setCreationTime(const unsigned long long int& creationTime);

    const unsigned long long int& expirationTime() const;
    void setExpirationTime(const unsigned long long int& expirationTime);

    const std::string& flags() const;
    void setFlags(const std::string& flags);


private:
    std::string _identifier;
    unsigned long long int _creation_time;
    unsigned long long int _expiration_time;
    std::string _flags;
};

class PublicKey
{
public:
    PublicKey(std::string       identifier = "",
	      unsigned int      algorithm  = 0,
	      unsigned int      key_length = 0,
	      unsigned int      creation_time = 0,
	      unsigned int      expiration_time = 0,
	      std::string       flags = "",
	      std::list<UserID> uids = std::list<UserID>())
	: _identifier(identifier),
	  _algorithm(algorithm),
	  _key_length(key_length),
	  _creation_time(creation_time),
	  _expiration_time(expiration_time),
	  _flags(flags),
	  _uids(uids) {}


    const std::string& identifier() const;
    void               setIdentifier(const std::string& identifier);

    const unsigned int& algorithm();
    void                setAlgorithm(const unsigned int& identifier); 

    const unsigned int& keyLength() const;
    void                setKeyLength(const unsigned int& keyLength);

    const unsigned long long int& creationTime() const;
    void setCreationTime(const unsigned long long int& creationTime);

    const unsigned long long int& expirationTime() const;
    void setExpirationTime(const unsigned long long int& expirationTime);

    const std::string& flags() const;
    void setFlags(const std::string& flags);

    const std::list<UserID>& uids() const;
    void addUid(UserID uid);


private:
    std::string _identifier;
    unsigned int _algorithm;
    unsigned int _key_length;
    unsigned long long int _creation_time;
    unsigned long long int _expiration_time;
    std::string _flags;
    std::list<UserID> _uids;
};

#endif // __KEYS_HXX
