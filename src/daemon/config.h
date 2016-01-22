// Copyright 2016 Lachlan Gunn

#ifndef KEYWATCH_DAEMON_CONFIG_H_
#define KEYWATCH_DAEMON_CONFIG_H_

#include <list>
#include <string>

namespace keywatch {
namespace daemon {

class Recipient {
 public:
  explicit Recipient(const std::string& email);

  const std::string& email() const;
  void setEmail(const std::string& rhs);

 private:
  std::string email_;
};

class Configuration {
 public:
  static Configuration&& fromFile(std::string filename);
  
 private:
  Configuration();
};

}  // namespace daemon
}  // namespace keywatch


#endif  // KEYWATCH_DAEMON_CONFIG_H_
