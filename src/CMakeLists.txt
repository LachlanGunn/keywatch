ADD_EXECUTABLE(keywatchd main.cc parser.cc keys.cc)
TARGET_LINK_LIBRARIES(keywatchd ${CURL_LIBRARIES})

MESSAGE(${CURL_LIBRARIES})
