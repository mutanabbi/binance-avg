cmake_minimum_required(VERSION 3.16)

project(
    binance-avg
    DESCRIPTION "An app to find the fastest websocket connetion to binance.com marketdata stream"
    VERSION 0.0.1
    LANGUAGES CXX
  )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.71.0 REQUIRED COMPONENTS
    system
)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Required Boost libraries not found. Please get a copy from http://www.boost.org")
endif()

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

# If CMAKE_BUILD_TYPE is not set, assume Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

add_subdirectory(src)
