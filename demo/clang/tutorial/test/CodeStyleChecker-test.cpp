// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin csc -verify %s 2>&1

// expected-warning@+2 {{Type and variable names should start with upper-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
class class_Bad;
class ClassOK;

// expected-warning@+2 {{Type and variable names should start with upper-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
struct struct_Bad;
struct StructOK;

// expected-warning@+2 {{Type and variable names should start with upper-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
union union_Bad;
union UnionOK;

// expected-warning@+2 {{Function names should start with lower-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
void Function_Bad();
void functionOK();

struct A {
  // expected-warning@+2 {{Function names should start with lower-case letter}}
  // expected-warning@+1 {{'_' in names is not allowed}}
  void Member_Method_Bad();
  void memberMethondOK();

  void operator()(int);
};

// expected-warning@+2 {{Type and variable names should start with upper-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
int int_Bad;
int IntOK;

struct B {
  // expected-warning@+2 {{Type and variable names should start with upper-case letter}}
  // expected-warning@+1 {{'_' in names is not allowed}}
  int member_Int_Bad;
  int MemberIntOK;
};

struct C {
  operator bool();
  operator A();
};
