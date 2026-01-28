add_llvm_component_library(LLVMToyInfo
  ToyTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  Toy
  )
