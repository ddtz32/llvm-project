set(Python3_EXECUTABLE "/usr/bin/env python3")
if(APPLE)
  set(Python3_EXECUTABLE "/opt/homebrew/bin/python3")
endif()
configure_file(
  "llvm-tutorial-lit.in"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/llvm-tutorial-lit"
  @ONLY
)
