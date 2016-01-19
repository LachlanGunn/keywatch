#include "keys.hxx"

const std::string& PublicKey::identifier() const
{
    return _identifier;
}

void PublicKey::setIdentifier(const std::string& identifier)
{
    _identifier = identifier;
}

const unsigned int& PublicKey::keyLength() const
{
    return _key_length;
}

void PublicKey::setKeyLength(const unsigned int& keyLength)
{
    _key_length = keyLength;
}

const unsigned long long int& PublicKey::creationTime() const
{
    return _creation_time;
}

void PublicKey::setCreationTime(const unsigned long long int& creationTime)
{
    _creation_time = creationTime;
}

const unsigned long long int& PublicKey::expirationTime() const
{
    return _expiration_time;
}

void PublicKey::setExpirationTime(const unsigned long long int& expirationTime)
{
    _expiration_time = expirationTime;
}

const std::string& PublicKey::flags() const
{
    return _flags;
}

const std::list<UserID>& PublicKey::uids() const
{
    return _uids;
}

void PublicKey::addUid(UserID uid)
{
    _uids.push_back(uid);
}

const std::string& UserID::identifier() const
{
    return _identifier;
}

void UserID::setIdentifier(const std::string& identifier)
{
    _identifier = identifier;
}

const unsigned long long int& UserID::creationTime() const
{
    return _creation_time;
}

void UserID::setCreationTime(const unsigned long long int& creationTime)
{
    _creation_time = creationTime;
}

const unsigned long long int& UserID::expirationTime() const
{
    return _expiration_time;
}

void UserID::setExpirationTime(const unsigned long long int& expirationTime)
{
    _expiration_time = expirationTime;
}

const std::string& UserID::flags() const
{
    return _flags;
}
