cmake_minimum_required(VERSION 3.19)
project(go-board-fun)

include(CTest)

find_package(verilator HINTS $ENV{VERILATOR_ROOT})
find_package(Catch2 REQUIRED)

include(CTest)

add_subdirectory(verilatest)

add_subdirectory(rtl)
