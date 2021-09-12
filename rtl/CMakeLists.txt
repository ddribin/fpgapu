set(BASE_NAME pwm)

set(LIB_TARGET_NAME ${BASE_NAME}_lib)
add_library(${LIB_TARGET_NAME} STATIC)
target_compile_features(${LIB_TARGET_NAME} PUBLIC cxx_std_11)

verilate(${LIB_TARGET_NAME} SOURCES pwm_tb.v TRACE)
verilate(${LIB_TARGET_NAME} SOURCES pwm_top.v)
verilate(${LIB_TARGET_NAME} SOURCES timing_strobe_generator.v)

include(Catch)

set(TEST_TARGET_NAME ${BASE_NAME}_tests)
add_executable(${TEST_TARGET_NAME}
    pwm_test.cpp
    ../common/test-common.hpp
    ../common/test-main.cpp
)

verilatest_setup(${TEST_TARGET_NAME})

target_include_directories(${TEST_TARGET_NAME} PRIVATE ../common)
target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    ${LIB_TARGET_NAME} Catch2::Catch2)
target_compile_definitions(${TEST_TARGET_NAME} PRIVATE VERILATEST_TRACE=1)
catch_discover_tests(${TEST_TARGET_NAME})
