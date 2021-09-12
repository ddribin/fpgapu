cmake_minimum_required(VERSION 3.19)
project(go-board-fun)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(verilator HINTS $ENV{VERILATOR_ROOT})
find_package(Catch2 REQUIRED)
find_package(SDL2 REQUIRED)

include(CTest)

add_subdirectory(verilatest)

add_subdirectory(rtl)
