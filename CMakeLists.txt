cmake_minimum_required(VERSION 3.30.0)
project(blackjack_ai VERSION 0.1.0 LANGUAGES C CXX)

add_executable(blackjack_ai agents.cpp environment.cpp game_assets.cpp main.cpp function.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# add_executable(
#     hello_test
#     hello_test.cc
# )
# target_link_libraries(
#     hello_test
#     GTest::gtest_main
# )

# Adds the necessary files for the environment unit test
add_executable(
    environment_unittest
    environment_unittest.cc
    environment.cpp
    game_assets.cpp
)


target_link_libraries(
    environment_unittest
    GTest::gtest_main
)

# Adds and links the necessary files for the function unit test
add_executable(
    function_unittest
    function_unittest.cc
    function.cpp
    environment.cpp
    game_assets.cpp
)

target_link_libraries(
    function_unittest
    GTest::gtest_main
)

# Adds and links the necessary files for the game_asset unit test
add_executable(
    game_assets_unittest
    game_assets_unittest.cc
    game_assets.cpp
)

target_link_libraries(
    game_assets_unittest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(game_assets_unittest)
gtest_discover_tests(environment_unittest)
gtest_discover_tests(function_unittest)