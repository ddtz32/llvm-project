macro(add_llvm_tutorial_library target)
  add_llvm_library(${target} SHARED PARTIAL_SOURCES_INTENDED ${target}.cpp)
endmacro()

add_llvm_tutorial_library(PrintSomething)
add_llvm_tutorial_library(OpcodeCounter)
