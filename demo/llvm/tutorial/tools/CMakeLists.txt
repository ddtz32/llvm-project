set(Python3_EXECUTABLE "/usr/bin/env python3")
if(APPLE)
  set(Python3_EXECUTABLE "/opt/homebrew/bin/python3")
endif()
configure_file(
  "llvm-tutorial-lit.in"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/llvm-tutorial-lit"
  @ONLY
)

add_llvm_executable(static-call-counter
  StaticCallCounterMain.cpp
  ../lib/StaticCallCounter.cpp
)
target_include_directories(static-call-counter PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)
llvm_map_components_to_libnames(llvm_libs core irreader support passes)
target_link_libraries(static-call-counter PRIVATE ${llvm_libs})
