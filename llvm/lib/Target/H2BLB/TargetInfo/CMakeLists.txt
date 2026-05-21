add_llvm_component_library(LLVMH2BLBInfo
  H2BLBTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  H2BLB
)
