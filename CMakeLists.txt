cmake_minimum_required(VERSION 3.10)

# Project name
project(CQLServer VERSION 1.0)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Disable response files
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES Off)

# Packages
find_package(Threads REQUIRED)

add_subdirectory(subprojects)
# add_subdirectory(tests)

# SRCS
AUX_SOURCE_DIRECTORY(lib/util SRCS)
message(STATUS "srcs: ${SRCS}")

# add the executable
add_executable(cql_server src/main.cc ${SRCS})
target_link_libraries(cql_server ${CMAKE_THREAD_LIBS_INIT} )
target_include_directories(cql_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/)


# add the tests
add_executable(logger_test tests/logger_test.cc ${SRCS})
add_executable(gtest_test tests/gtest_test.cc ${SRCS})

target_link_libraries(gtest_test gtest_main)
target_link_libraries(logger_test gtest_main)

target_include_directories(gtest_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
target_include_directories(logger_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/)

include(GoogleTest)
gtest_discover_tests(gtest_test)