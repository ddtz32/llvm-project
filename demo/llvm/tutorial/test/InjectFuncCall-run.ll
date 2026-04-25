; RUN: opt -load-pass-plugin %lib/libInjectFuncCall%ext \
; RUN:   -passes="inject-func-call,verify" %S/Inputs/Call.ll -o %t
; RUN: lli %t | FileCheck %s

; CHECK:          Hello from main, number of arguments: 2
; CHECK-NEXT:     Hello from foo, number of arguments: 0

; CHECK-NEXT:     Hello from bar, number of arguments: 0
; CHECK-NEXT:     Hello from foo, number of arguments: 0

; CHECK-NEXT:     Hello from fez, number of arguments: 0
; CHECK-NEXT:     Hello from bar, number of arguments: 0
; CHECK-NEXT:     Hello from foo, number of arguments: 0

; CHECK-COUNT-10: Hello from foo, number of arguments: 0
