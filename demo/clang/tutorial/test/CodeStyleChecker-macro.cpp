// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin csc -verify %s 2>&1

#define ADD_SUFFIX(name) name##Sufiix

// expected-warning@+2 {{Type and variable names should start with upper-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
class ADD_SUFFIX(class_Bad);
class ADD_SUFFIX(ClassOK);

// expected-warning@+2 {{Type and variable names should start with upper-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
struct ADD_SUFFIX(struct_Bad);
struct ADD_SUFFIX(StructOK);

// expected-warning@+2 {{Type and variable names should start with upper-case letter}}
// expected-warning@+1 {{'_' in names is not allowed}}
union ADD_SUFFIX(union_Bad);
union ADD_SUFFIX(UnionOK);
