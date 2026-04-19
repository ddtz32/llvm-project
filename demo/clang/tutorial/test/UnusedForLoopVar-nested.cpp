// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin uflv -verify %s

int test() {
  int a = 10;

  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable 'i' not used}}
  for (int i = 0; i < 20; i++)
    // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable 'j' not used}}
    for (int j = 0; j < 30; j++)
      // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable 'k' not used}}
      for (int k = 0; k < 40; k++)
        a++;

  return a;
}
