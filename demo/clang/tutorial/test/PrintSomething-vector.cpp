// RUN: %clang++ -Xclang -load -Xclang %lib/libClangTutorial.so \
// RUN: -Xclang -plugin -Xclang print-something -c %s

#include <vector>
