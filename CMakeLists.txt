cmake_minimum_required(VERSION 3.16.0)
project(netship VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)

# default compiler options and warnings
include(cmake/DefaultCompilerOptionsAndWarnings.cmake)

# all the hpp files
file(GLOB HPPS RELATIVE ${PROJECT_SOURCE_DIR}
    src/*.hpp
)

# include (asio) library
include_directories(include/)

# setup Catch2 testing v3.0.0-preview5

# get it using FetchContent so user doesn't have to do any work installing it
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview5
)
FetchContent_MakeAvailable(Catch2)
# this line is specific for FetchContent way (instead of find_package)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# find_package(Catch2 3 REQUIRED) # this would be used if used had installed the library
add_executable(tests src/tests.cpp ${HPPS})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Make Catch connected with Ctest
include(CTest)
include(Catch)
catch_discover_tests(tests)
enable_testing()

# setup main game executable
add_executable(netship src/main.cpp ${HPPS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)