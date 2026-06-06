cmake_minimum_required(VERSION 3.20)

project(FestivalTrackerFirmware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(festival_tracker_core
    src/festival_tracker/core/ButtonPattern.cpp
    src/festival_tracker/core/FlashPolicy.cpp
    src/festival_tracker/core/GnssPolicy.cpp
    src/festival_tracker/core/ImuPolicy.cpp
    src/festival_tracker/core/LedStateMachine.cpp
    src/festival_tracker/core/NodeTable.cpp
    src/festival_tracker/core/Sequence.cpp
    src/festival_tracker/core/SleepScheduler.cpp
    src/festival_tracker/core/SosState.cpp
    src/festival_tracker/core/SyncPolicy.cpp
    src/festival_tracker/core/TableDigest.cpp
    src/festival_tracker/core/Types.cpp
)

target_include_directories(festival_tracker_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(festival_tracker_adapters
    src/festival_tracker/adapters/FakeClockAdapter.cpp
    src/festival_tracker/adapters/FakeGnssAdapter.cpp
    src/festival_tracker/adapters/FakeImuAdapter.cpp
    src/festival_tracker/adapters/FakeLedAdapter.cpp
    src/festival_tracker/adapters/FakeRadioAdapter.cpp
    src/festival_tracker/adapters/FakeStorageAdapter.cpp
)

target_link_libraries(festival_tracker_adapters
    PUBLIC
        festival_tracker_core
)

enable_testing()

function(add_native_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name}
        PRIVATE
            festival_tracker_adapters
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

add_native_test(test_node_table tests/native/test_node_table.cpp)
add_native_test(test_sequence tests/native/test_sequence.cpp)
add_native_test(test_table_digest tests/native/test_table_digest.cpp)
add_native_test(test_sync_policy tests/native/test_sync_policy.cpp)
add_native_test(test_sleep_scheduler tests/native/test_sleep_scheduler.cpp)
add_native_test(test_gnss_policy tests/native/test_gnss_policy.cpp)
add_native_test(test_imu_policy tests/native/test_imu_policy.cpp)
add_native_test(test_led_state_machine tests/native/test_led_state_machine.cpp)
add_native_test(test_sos tests/native/test_sos.cpp)
add_native_test(test_flash_policy tests/native/test_flash_policy.cpp)
