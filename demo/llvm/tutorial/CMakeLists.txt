cmake_minimum_required(VERSION 3.25)

project(llvm-tutorial)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")

if(NOT LLVM_ROOT)
  set(LLVM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../build")
endif()
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_definitions("${LLVM_DEFINITIONS}")
include_directories("${LLVM_INCLUDE_DIRS}")

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(tools)
