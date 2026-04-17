// RUN: %clang++ -Xclang -load -Xclang %lib/libClangTutorial%ext \
// RUN: -Xclang -plugin -Xclang print-something -c %s

#include <vector>
