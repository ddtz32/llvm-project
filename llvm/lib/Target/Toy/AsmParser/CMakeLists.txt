add_llvm_component_library(LLVMToyAsmParser
  ToyAsmParser.cpp

  LINK_COMPONENTS
  MCParser
  Support
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )
