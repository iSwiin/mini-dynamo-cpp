cmake_minimum_required(VERSION 3.20)
project(mini_dynamo_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

include(cmake/Sanitizers.cmake)

add_library(dynamo_client
  src/client.cpp
  src/config.cpp
  src/hash.cpp
  src/http.cpp
  src/ring.cpp
)

target_include_directories(dynamo_client PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Warnings (keep reasonable)
if (MSVC)
  target_compile_options(dynamo_client PRIVATE /W4)
else()
  target_compile_options(dynamo_client PRIVATE -Wall -Wextra -Wpedantic)
endif()

# libcurl for HTTP
find_package(CURL REQUIRED)
target_link_libraries(dynamo_client PUBLIC CURL::libcurl)

enable_sanitizers(dynamo_client)

add_executable(dynamoctl tools/dynamoctl.cpp)
target_link_libraries(dynamoctl PRIVATE dynamo_client)
enable_sanitizers(dynamoctl)

# ---- tests ----
include(CTest)
if (BUILD_TESTING)
  add_executable(unit_tests
    tests/test_main.cpp
    tests/test_hash.cpp
    tests/test_ring.cpp
    tests/test_config.cpp
  )
  target_link_libraries(unit_tests PRIVATE dynamo_client)
  enable_sanitizers(unit_tests)
  add_test(NAME unit_tests COMMAND unit_tests)
endif()
