// Copyright 2016 Lachlan Gunn

#include "hkp/hkp.h"

#include <string>
#include <mutex>

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

static uint64_t connection_number = 0;
static std::mutex connection_number_mutex;

}

HKPServer::HKPServer(std::string host, std::string proxy) : host_(host) {
  state_ = std::unique_ptr<struct HKPServerState_>(new struct HKPServerState_);
  state_->curl_handle = curl_easy_init();
  setProxy(proxy);

  curl_easy_setopt(state_->curl_handle, CURLOPT_WRITEFUNCTION,
                   _HKPWriteCallback);

  curl_easy_setopt(state_->curl_handle, CURLOPT_FOLLOWLOCATION,
                   1);

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
}

const std::list<PublicKey> HKPServer::GetKeys(std::string email) {
  HKPResponseParser parser;

  std::string url = (boost::format("%1%/pks/lookup?op=index&options=mr"
                                   "&search=<%2%>") % host_ % email).str();

  curl_easy_setopt(state_->curl_handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(state_->curl_handle, CURLOPT_WRITEDATA, &parser);

  std::string proxy_url;

  if (!proxy_.empty()) {
    // The connection number is global, we need to lock for thread-safety.
    {
      std::lock_guard<std::mutex> lock(connection_number_mutex);
      proxy_url = (boost::format("%1%:password@%2%")
                   % connection_number++ % proxy_).str();
    }
    curl_easy_setopt(state_->curl_handle, CURLOPT_PROXY, proxy_url.c_str());
    curl_easy_setopt(state_->curl_handle, CURLOPT_PROXYTYPE,
                     CURLPROXY_SOCKS5_HOSTNAME);
  }
  else {
    curl_easy_setopt(state_->curl_handle, CURLOPT_PROXY, "");
  }

  CURLcode curl_error = curl_easy_perform(state_->curl_handle);

  if (curl_error) {
    std::cerr << "ERROR: " << curl_easy_strerror(curl_error) << "\n";
    return std::list<PublicKey>();
  }
  parser.flush();
  return parser.keys();
}

}  //   namespace hkp
}  // namespace keywatch
