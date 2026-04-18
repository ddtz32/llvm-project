// RUN: %clang++ -Xclang -load -Xclang %lib/libClangTutorial%ext \
// RUN: -Xclang -plugin -Xclang obfuscator -c %s

#include <vector>
