cmake_minimum_required(VERSION 3.25)

project(clang-tutorial)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")

if(NOT Clang_ROOT)
  set(Clang_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../build")
endif()
find_package(Clang REQUIRED CONFIG)
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CLANG_CMAKE_DIR}")
include(AddLLVM)
include(AddClang)

add_definitions("${LLVM_DEFINITIONS}")
include_directories("${LLVM_INCLUDE_DIRS}")
include_directories("${CLANG_INCLUDE_DIRS}")

include_directories("include")

add_subdirectory(lib)
add_subdirectory(test)
