cmake_minimum_required(VERSION 3.26)
project(VMFOverview CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories("D:/boost_1_82_0")
link_directories("D:/boost_1_82_0/stage/lib")
set(BOOST_ROOT "D:/boost_1_82_0")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost 1.82.0 REQUIRED)

add_executable(VMFOverview main.cpp
        utils.cpp
        utils.h)
