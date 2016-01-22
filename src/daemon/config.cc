#include "daemon/config.h"

#include <list>
#include <string>

namespace keywatch {
namespace daemon {

Recipient::Recipient(const std::string& email) : email_(email) {}

const std::string& Recipient::email() const {
  return email_;
}

void Recipient::setEmail(const std::string& email) {
  email_ = email;
}

} // namespace daemon
} // namespace daemon
