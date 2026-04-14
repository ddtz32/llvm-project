// RUN: %clang++ -Xclang -load -Xclang %lib/libClangTutorial.so \
// RUN: -Xclang -plugin -Xclang lac -c %s

#include <vector>
