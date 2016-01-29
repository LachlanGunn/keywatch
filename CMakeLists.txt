CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(keywatch)

#### Configuration ############################################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#### Libraries ################################################################

# Boost (for the Spirit parser)
FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options)
INCLUDE_DIRECTORIES(Boost_INCLUDE_DIRS)

# CURL (for http)
FIND_PACKAGE(CURL REQUIRED)
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})

# Crypto++ (for hashing and random-number generation)
FIND_LIBRARY(CRYPTOPP_LIBRARIES crypto++)
IF(NOT CRYPTOPP_LIBRARIES)
  MESSAGE(SEND_ERROR "Could not find Crypto++")
ENDIF(NOT CRYPTOPP_LIBRARIES)

#### Down to business #########################################################

SUBDIRS(src)
