// RUN: %clang -cc1 -load %lib/libPrintSomething.so -plugin print-something %s \
// RUN:   2>&1 | FileCheck %s

// CHECK: count: 1, file: {{.*}}/demo/clang/tutorial/test/PrintSomething-macro.cpp
// CHECK: count: 1, file: {{.*}}/demo/clang/tutorial/test/Inputs/PrintSomething-macro.h

#include "Inputs/PrintSomething-macro.h"

clang_tutorial_class(Bar) {};

