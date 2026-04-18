// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin print-something %s \
// RUN:   2>&1 | FileCheck %s

// CHECK: count: 4, file: {{.*}}/demo/clang/tutorial/test/PrintSomething-test.cpp
// CHECK: count: 1, file: {{.*}}/demo/clang/tutorial/test/Inputs/PrintSomething-test.h


class A {
  int a;
};

struct B {
  float b;
};

union C {
  A a;
  B b;
};

#include "Inputs/PrintSomething-test.h"

clang_tutorial_class(Bar) {};
