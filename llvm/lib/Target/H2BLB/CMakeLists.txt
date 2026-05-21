add_llvm_component_group(H2BLB)

add_llvm_target(H2BLBCodeGen
  H2BLBISelLowering.cpp
  H2BLBSubtarget.cpp
  H2BLBTargetMachine.cpp

  LINK_COMPONENTS
  H2BLBInfo

  CodeGen
  Core
  MC
  SelectionDAG
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  H2BLB
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
