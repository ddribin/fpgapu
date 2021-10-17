set(BASE_NAME fpgapu)

set(LIB_TARGET_NAME ${BASE_NAME}_lib)
add_library(${LIB_TARGET_NAME} STATIC)
target_compile_features(${LIB_TARGET_NAME} PUBLIC cxx_std_11)

verilate(${LIB_TARGET_NAME} SOURCES pwm.v TRACE
    VERILATOR_ARGS -GRESOLUTION=4)
verilate(${LIB_TARGET_NAME} SOURCES pwm_top.v)
verilate(${LIB_TARGET_NAME} SOURCES timing_strobe_generator.v)
verilate(${LIB_TARGET_NAME} SOURCES note_sequencer_tb.v TRACE)
verilate(${LIB_TARGET_NAME} SOURCES pattern_sequencer_tb.v TRACE)
verilate(${LIB_TARGET_NAME} SOURCES note_player_tb.v TRACE)
verilate(${LIB_TARGET_NAME} SOURCES duration_counter.v TRACE)
verilate(${LIB_TARGET_NAME} SOURCES pitch_lookup_tb.v TRACE)
verilate(${LIB_TARGET_NAME} SOURCES channel_controller.v TRACE)
verilate(${LIB_TARGET_NAME} SOURCES rom_sync_fake.v TRACE
    VERILATOR_ARGS -GWIDTH=16 -GDEPTH=17)

set(SDL_TARGET_NAME FPGAPU)
add_executable(${SDL_TARGET_NAME} fpgapu_sdl_main.cpp)
verilate(${SDL_TARGET_NAME} SOURCES fpgapu_sdl_top.v)
target_link_libraries(${SDL_TARGET_NAME} PRIVATE SDL2::Main)

add_custom_target(${SDL_TARGET_NAME}-resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(${SDL_TARGET_NAME} ${SDL_TARGET_NAME}-resources)

set(CHANNEL_TEST_TARGET_NAME channel_test)
add_executable(${CHANNEL_TEST_TARGET_NAME} channel_test_sdl_main.cpp)
verilate(${CHANNEL_TEST_TARGET_NAME} SOURCES channel_test_sdl_top.v)
target_link_libraries(${CHANNEL_TEST_TARGET_NAME} PRIVATE SDL2::Main)

include(Catch)

set(TEST_TARGET_NAME ${BASE_NAME}_tests)
add_executable(${TEST_TARGET_NAME}
    pwm_test.cpp
    note_sequencer_test.cpp
    pattern_sequencer_test.cpp
    note_player_test.cpp
    duration_counter_test.cpp
    pitch_lookup_test.cpp
    channel_controller_test.cpp
    rom_sync_fake_test.cpp
    ../common/test-listener.cpp
    ../common/test-main.cpp
)

verilatest_setup(${TEST_TARGET_NAME})

target_include_directories(${TEST_TARGET_NAME} PRIVATE ../common)
target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    ${LIB_TARGET_NAME} Catch2::Catch2)
target_compile_definitions(${TEST_TARGET_NAME} PRIVATE VERILATEST_TRACE=1)
catch_discover_tests(${TEST_TARGET_NAME})
