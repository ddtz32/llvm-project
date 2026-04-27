; RUN: opt -load-pass-plugin %lib/libDynamicCallCounter%ext \
; RUN:   -passes="dynamic-cc,verify" %S/Inputs/Call.ll -o %t
; RUN: lli %t 2>&1 | FileCheck %s

; CHECK:      foo  13
; CHECK-NEXT: bar  2
; CHECK-NEXT: fez  1
; CHECK-NEXT: main 1
