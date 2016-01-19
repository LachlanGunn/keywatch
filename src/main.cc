// Copyright 2016 Lachlan Gunn

#include <stdio.h>

#include <string>
#include <iostream>

extern "C" {
#include <curl/curl.h> // NOLINT
}

#include "parser.h"
#include "keys.h"

size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp) {
  HKPResponseParser* parser = static_cast<HKPResponseParser*>(userp);
  parser->parseCharacters(std::string((const char*)buffer, size*nmemb));

  return size*nmemb;
}

int main(int argc, char** argv) {
  CURL* curl_handle;
  int success;

  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();

  curl_easy_setopt(curl_handle, CURLOPT_URL,
                   "http://jirk5u4osbsr34t5.onion:11371/pks/lookup"
                   "?op=index&options=mr&search=lachlan@twopif.net");

  curl_easy_setopt(curl_handle, CURLOPT_PROXY, "localhost:9050");
  curl_easy_setopt(curl_handle, CURLOPT_PROXYTYPE,
                   CURLPROXY_SOCKS5_HOSTNAME);


  HKPResponseParser parser;

  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,     &parser);

  success = curl_easy_perform(curl_handle);
  parser.flush();

  if (success) {
    printf("Failed to download index: %d\n", success);
  }

  std::list<PublicKey> keys = parser.keys();

  std::list<PublicKey>::const_iterator iterator, end;
  for (iterator = keys.begin(), end = keys.end();
       iterator != end;
       iterator++) {
    printf("Key: %s\n", (*iterator).identifier().c_str());

    std::list<UserID>::const_iterator iterator_uid, end_uid;
    std::list<UserID> uids = iterator->uids();
    for (iterator_uid = uids.begin(), end_uid = uids.end();
         iterator_uid != end_uid;
         iterator_uid++) {
      printf("    UID: %s\n", iterator_uid->identifier().c_str());
    }
  }


  curl_global_cleanup();
  return 0;
}
