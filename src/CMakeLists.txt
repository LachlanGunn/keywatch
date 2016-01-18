ADD_EXECUTABLE(keywatchd main.cxx parser.cxx keys.cxx)
TARGET_LINK_LIBRARIES(keywatchd ${CURL_LIBRARIES})

MESSAGE(${CURL_LIBRARIES})
