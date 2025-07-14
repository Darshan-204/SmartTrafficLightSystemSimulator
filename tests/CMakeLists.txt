cmake_minimum_required(VERSION 3.10)

# Download and configure GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(traffic_tests test_traffic.cpp)

# Link against GoogleTest and our project's library
target_link_libraries(traffic_tests 
    PRIVATE 
    gtest_main
    ${CMAKE_PROJECT_NAME}_lib
)

# Register tests
add_test(NAME traffic_tests COMMAND traffic_tests)
