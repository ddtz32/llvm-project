# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
# llvm handle this in `llvm/cmake/modules/HandleLLVMOptions.cmake`
link_libraries("$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")

macro(add_llvm_tutorial_library target)
  add_llvm_library(${target} SHARED PARTIAL_SOURCES_INTENDED ${target}.cpp)
endmacro()

add_llvm_tutorial_library(PrintSomething)
add_llvm_tutorial_library(OpcodeCounter)
add_llvm_tutorial_library(InjectFuncCall)

add_llvm_tutorial_library(StaticCallCounter)
target_include_directories(StaticCallCounter PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")

add_llvm_tutorial_library(DynamicCallCounter)
add_llvm_tutorial_library(MBASub)
add_llvm_tutorial_library(MBAAdd)
