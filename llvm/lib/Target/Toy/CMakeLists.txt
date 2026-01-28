add_llvm_component_group(Toy)

add_llvm_target(ToyCodeGen
  ToyTargetMachine.cpp

  LINK_COMPONENTS
  CodeGen
  MC
  Support
  Target
  TargetParser
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
