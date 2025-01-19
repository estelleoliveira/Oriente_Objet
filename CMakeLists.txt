cmake_minimum_required(VERSION 3.20)

# set the project name
project(simulator)

# enable modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_MODE)
endif()

# add the executable
add_executable(simulator main.cpp Problem.cpp Equation.cpp)

#find_package(Threads)
#find_package(TBB)
#target_link_libraries(simulator Threads::Threads TBB::tbb)


# Find GTest 
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# add the test executable
add_executable(tests test.cpp Problem.cpp Equation.cpp)

# Enable testing
enable_testing()

target_link_libraries(tests GTest::GTest GTest::Main pthread)
gtest_discover_tests(tests)
