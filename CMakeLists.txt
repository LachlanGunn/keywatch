CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(keywatchd)

#### Configuration ############################################################

SET(CMAKE_CXX_FLAGS -g)

#### Libraries ################################################################

# Boost (for the Spirit parser)
FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES(Boost_INCLUDE_DIRS)

# CURL (for http)
FIND_PACKAGE(CURL REQUIRED)
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})

#### Down to business #########################################################

SUBDIRS(src)