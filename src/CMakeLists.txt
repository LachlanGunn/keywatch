INCLUDE_DIRECTORIES(.)

ADD_EXECUTABLE(keywatchd 
  main.cc
  keys/keys.cc
  hkp/hkp.cc
  hkp/parser.cc
  daemon/config.cc)

TARGET_LINK_LIBRARIES(keywatchd ${CURL_LIBRARIES})

MESSAGE(${CURL_LIBRARIES})
