; RUN: opt -load-pass-plugin %lib/libStaticCallCounter%ext \
; RUN:   -passes="print<static-cc>" -disable-output %S/Inputs/Call.ll \
; RUN:   2>&1 | FileCheck %s

; RUN: ../bin/static-call-counter %S/Inputs/Call.ll 2>&1 | FileCheck %s

; CHECK: foo 3
; CHECK: bar 2
; CHECK: fez 1
