#include "daemon/config.h"

#include <list>
#include <string>

namespace keywatch {
namespace daemon {

Recipient::Recipient(const std::string& email, const std::string& keyserver,
                     const std::string& proxy)
    : email_(email), keyserver_(keyserver), proxy_(proxy) {}

const std::string& Recipient::email() const {
  return email_;
}

void Recipient::setEmail(const std::string& email) {
  email_ = email;
}

const std::string& Recipient::keyserver() const {
  return keyserver_;
}

void Recipient::setKeyserver(const std::string& keyserver) {
  keyserver_ = keyserver;
}

const std::string& Recipient::proxy() const {
  return proxy_;
}

void Recipient::setProxy(const std::string& proxy) {
  proxy_ = proxy;
}

Configuration::Configuration() {}

const std::list<Recipient>& Configuration::recipients() const {
  return recipients_;
}

} // namespace daemon
} // namespace daemon
