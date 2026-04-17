set(Python3_EXECUTABLE "/usr/bin/env python3")
if(APPLE)
  set(Python3_EXECUTABLE "/opt/homebrew/bin/python3")
endif()
configure_file(
  "clang-tutorial-lit.in"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/clang-tutorial-lit"
  @ONLY
)

add_clang_executable(la-commenter
  LACommenterMain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/LACommenter.cpp
)
target_link_libraries(la-commenter PRIVATE
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
