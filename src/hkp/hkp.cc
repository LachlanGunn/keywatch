// Copyright 2016 Lachlan Gunn

#include "hkp/hkp.h"

#include <string>

extern "C" {
#include <curl/curl.h> // NOLINT
}

#include <boost/format.hpp> //NOLINT

#include "hkp/parser.h"

namespace keywatch {
namespace hkp {

using keywatch::keys::PublicKey;
using keywatch::keys::UserID;

struct keywatch::hkp::HKPServer::HKPServerState_ {
  CURL* curl_handle;
};

bool HKPInit() {
  CURLcode response = curl_global_init(CURL_GLOBAL_ALL);
  return (response == 0);
}

void HKPCleanup() {
  curl_global_cleanup();
}

namespace {

// Callback for CURL
static size_t _HKPWriteCallback(void* buffer, size_t size, size_t nmemb,
                                void* userp) {
  HKPResponseParser* parser = static_cast<HKPResponseParser*>(userp);
  parser->parseCharacters(std::string((const char*)buffer, size*nmemb));

  return size*nmemb;
}

}

HKPServer::HKPServer(std::string host, std::string proxy) : host_(host) {
  state_ = std::unique_ptr<struct HKPServerState_>(new struct HKPServerState_);
  state_->curl_handle = curl_easy_init();
  setProxy(proxy);

  curl_easy_setopt(state_->curl_handle, CURLOPT_WRITEFUNCTION,
                   _HKPWriteCallback);
}

HKPServer::~HKPServer() {
  curl_easy_cleanup(state_->curl_handle);
}

const std::string& HKPServer::host() const {
  return host_;
}

void HKPServer::setHost(const std::string& host) {
  host_ = host;
}

const std::string& HKPServer::proxy() const {
  return proxy_;
}

void HKPServer::setProxy(const std::string& proxy) {
  proxy_ = proxy;
  curl_easy_setopt(state_->curl_handle, CURLOPT_PROXY, proxy.c_str());
  curl_easy_setopt(state_->curl_handle, CURLOPT_PROXYTYPE,
              CURLPROXY_SOCKS5_HOSTNAME);
}

const std::list<PublicKey> HKPServer::GetKeys(std::string email) {
  HKPResponseParser parser;

  std::string url = (boost::format("%1%/pks/lookup?op=index&options=mr"
                                   "&search=<%2%>") % host_ % email).str();

  curl_easy_setopt(state_->curl_handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(state_->curl_handle, CURLOPT_WRITEDATA, &parser);

  int curl_error = curl_easy_perform(state_->curl_handle);

  if (curl_error) {
    return std::list<PublicKey>();
  }
  parser.flush();
  return parser.keys();
}

}  //   namespace hkp
}  // namespace keywatch
