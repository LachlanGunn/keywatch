INCLUDE_DIRECTORIES(.)

ADD_EXECUTABLE(keywatchd 
  main.cc
  keys/keys.cc
  hkp/hkp.cc
  hkp/parser.cc
  daemon/config.cc
  daemon/status.cc
  daemon/worker.cc)

TARGET_LINK_LIBRARIES(keywatchd
  ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${CRYPTOPP_LIBRARIES}
  ${CURSES_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
