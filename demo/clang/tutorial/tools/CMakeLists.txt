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
