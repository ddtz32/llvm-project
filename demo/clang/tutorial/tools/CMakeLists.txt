set(Python3_EXECUTABLE "/usr/bin/env python3")
if(APPLE)
  set(Python3_EXECUTABLE "/opt/homebrew/bin/python3")
endif()
configure_file(
  "clang-tutorial-lit.in"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/clang-tutorial-lit"
  @ONLY
)

macro(add_clang_tutorial_tools target)
  add_clang_executable(${target} ${${target}-sources} PARTIAL_SOURCES_INTENDED)
  target_link_libraries(${target} PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangRewrite
  clangSerialization
  clangTooling
  LLVMFrontendOpenMP
  LLVMSupport
)
endmacro()

set(la-commenter-sources
  LACommenterMain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/LACommenter.cpp
)
add_clang_tutorial_tools(la-commenter)

set(code-style-checker-sources
  CodeStyleCheckerMain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/CodeStyleChecker.cpp
)
add_clang_tutorial_tools(code-style-checker)
