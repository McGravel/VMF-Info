cmake_minimum_required(VERSION 3.25)
project(VMFOverview CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories("include/boost_1_82_0")
link_directories("include/boost_1_82_0/stage/lib")
set(BOOST_ROOT "include/boost_1_82_0")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost 1.82.0 REQUIRED)

add_executable(VMFOverview src/main.cpp
        src/utils.cpp
        src/utils.h)
