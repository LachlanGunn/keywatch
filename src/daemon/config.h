// Copyright 2016 Lachlan Gunn

#ifndef KEYWATCH_DAEMON_CONFIG_H_
#define KEYWATCH_DAEMON_CONFIG_H_

#include <list>
#include <string>

namespace keywatch {
namespace daemon {

class Recipient {
 public:
  Recipient(const std::string& email, const std::string& keyserver,
            const std::string& proxy);

  const std::string& email() const;
  void setEmail(const std::string& rhs);

  const std::string& keyserver() const;
  void setKeyserver(const std::string& rhs);

  const std::string& proxy() const;
  void setProxy(const std::string& rhs);

 private:
  std::string email_;
  std::string keyserver_;
  std::string proxy_;
};

class Configuration {
 public:
  Configuration();

  const std::list<Recipient>& recipients() const;

  static Configuration fromFile(std::string filename);
  
 private:
  std::list<Recipient> recipients_;
};

}  // namespace daemon
}  // namespace keywatch


#endif  // KEYWATCH_DAEMON_CONFIG_H_
