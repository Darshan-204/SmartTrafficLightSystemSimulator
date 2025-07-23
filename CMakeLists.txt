cmake_minimum_required(VERSION 3.10)
project(SmartTrafficLight VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files (excluding main.cpp)
file(GLOB_RECURSE LIB_SOURCES "src/[!main]*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

# Create library target
add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}_lib PUBLIC include)

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# New: Interactive test executable
# add_executable(interactive_test interaction/userTest.cpp)
# target_link_libraries(interactive_test PRIVATE ${PROJECT_NAME}_lib)

# Threading support
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC Threads::Threads)
# target_link_libraries(interactive_test PRIVATE Threads::Threads)

# Enable testing
enable_testing()
add_subdirectory(tests)
