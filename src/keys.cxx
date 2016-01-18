#include "keys.hxx"

const std::string& PublicKey::identifier() const
{
    return _identifier;
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
