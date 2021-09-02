cmake_minimum_required(VERSION 3.16)

# If CMAKE_BUILD_TYPE is not set, assume Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

project(
    binance-avg
    VERSION 0.0.1
    DESCRIPTION "An app to find the fastest websocket connetion to binance.com marketdata stream"
    HOMEPAGE_URL "https://github.com/mutanabbi/binance-avg"
    LANGUAGES CXX
  )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(OpenSSL REQUIRED)

find_package(Threads REQUIRED)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.71.0 REQUIRED COMPONENTS
    system
)

find_package(GTest)
include(CTest)

add_compile_options(
    # TODO Wrap into genex if other than GCC/Clang compilers needed
    -Wall
    -Wextra
    -Wpedantic
  )

add_subdirectory(src)
