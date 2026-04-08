// RUN: %clang -cc1 -load %lib/libClangTutorial.so -plugin print-something %s \
// RUN:   -plugin-arg-print-something help \
// RUN:   -plugin-arg-print-something -parse-template \
// RUN:   -plugin-arg-print-something test \
// RUN:   2>&1 | FileCheck %s -check-prefix=CHECK-NORMAL

// CHECK-NORMAL: PrintSomething arg = help
// CHECK-NORMAL: PrintSomething arg = -parse-template
// CHECK-NORMAL: Help for PrintSomething plguin

// RUN: not %clang -cc1 -load %lib/libClangTutorial.so -plugin print-something %s \
// RUN:   -plugin-arg-print-something -an-error \
// RUN:   2>&1 | FileCheck %s -check-prefix=CHECK-AN-ERROR

// CHECK-AN-ERROR: PrintSomething arg = -an-error
// CHECK-AN-ERROR: invalid argument '-an-error'

// RUN: not %clang -cc1 -load %lib/libClangTutorial.so -plugin print-something %s \
// RUN:   -plugin-arg-print-something -parse-template \
// RUN:   2>&1 | FileCheck %s -check-prefix=CHECK-MISSING

// CHECK-MISSING: PrintSomething arg = -parse-template
// CHECK-MISSING: missing -parse-template argument
