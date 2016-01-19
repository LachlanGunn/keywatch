// Copyright Lachlan Gunn 2016

#ifndef KEYWATCHD_HKP_HKP_H_
#define KEYWATCHD_HKP_HKP_H_

#include <stdint.h>

#include <string>
#include <list>
#include <memory>

#include "keys/keys.h"
  
extern bool HKPInit();
extern void HKPCleanup();

struct _HKPServerState;

class HKPServer {
 public:
  explicit HKPServer(std::string host, std::string proxy = "");
  ~HKPServer();

  const std::list<PublicKey> GetKeys(std::string email);

  const std::string& host() const;
  void setHost(const std::string& host);

  const std::string& proxy() const;
  void setProxy(const std::string& proxy);

 private:
  struct HKPServerState_;

  HKPServer(HKPServer& rhs);
  HKPServer& operator=(HKPServer& rhs);

  std::string host_;
  std::string proxy_;
  std::unique_ptr<struct HKPServerState_> state_;
};

#endif  // KEYWATCHD_HKP_HKP_H_
