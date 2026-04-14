// RUN: %clang++ -Xclang -load -Xclang %lib/libClangTutorial.so -Xclang -plugin -Xclang csc -c %s
// RUN: %clang++ -Xclang -load -Xclang %lib/libClangTutorial.so -Xclang -plugin -Xclang csc -Xclang -plugin-arg-csc -Xclang -main-file-only=true -c %s
// RUN: %clang++ -Xclang -load -Xclang %lib/libClangTutorial.so -Xclang -plugin -Xclang csc -Xclang -plugin-arg-csc -Xclang -main-file-only=false -c %s

#include <vector>
