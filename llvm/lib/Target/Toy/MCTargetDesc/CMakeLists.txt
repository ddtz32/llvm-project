add_llvm_component_library(LLVMToyDesc
  ToyTargetDesc.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  Toy
  )
