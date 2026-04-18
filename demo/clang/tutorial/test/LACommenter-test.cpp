// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin lac %s \
// RUN:   2>&1 | FileCheck %s

// RUN: ../bin/la-commenter %s 2>&1 | FileCheck %s

// CHECK-LABEL: test()
// CHECK-NEXT: func(/*BoolArg*/true);
// CHECK-NEXT: func(/*CharArg*/'a');
// CHECK-NEXT: func(/*FloatArg*/1.0f);
// CHECK-NEXT: func(/*IntArg*/1);
// CHECK-NEXT: func(/*StringArg*/"hello");
// CHECK-NEXT: func(Arg);

void func(bool BoolArg);
void func(char CharArg);
void func(float FloatArg);
void func(int IntArg);
void func(const char *StringArg);

struct A {
  void operator()(int Arg);
};

void test(int Arg) {
  func(true);
  func('a');
  func(1.0f);
  func(1);
  func("hello");
  func(Arg);

  A a;
  a(1);
}
