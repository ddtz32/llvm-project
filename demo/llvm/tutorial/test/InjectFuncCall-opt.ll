; RUN: opt -load-pass-plugin %lib/libInjectFuncCall%ext \
; RUN:   -passes="inject-func-call,verify" -S %S/Inputs/Call.ll \
; RUN:   2>&1 | FileCheck %s

; CHECK:       @FormatStr = global [40 x i8] c"Hello from %s, number of arguments: %d\0A\00"
; CHECK-NEXT:  @0 = private unnamed_addr constant [4 x i8] c"foo\00", align 1
; CHECK-NEXT:  @1 = private unnamed_addr constant [4 x i8] c"bar\00", align 1
; CHECK-NEXT:  @2 = private unnamed_addr constant [4 x i8] c"fez\00", align 1
; CHECK-NEXT:  @3 = private unnamed_addr constant [5 x i8] c"main\00", align 1

; CHECK-LABEL: define void @foo()
; CHECK-NEXT:  %1 = call i32 (ptr, ...) @printf(ptr @FormatStr, ptr @0, i32 0)

; CHECK-LABEL: define void @bar()
; CHECK-NEXT:  %1 = call i32 (ptr, ...) @printf(ptr @FormatStr, ptr @1, i32 0)

; CHECK-LABEL: define void @fez()
; CHECK-NEXT:  %1 = call i32 (ptr, ...) @printf(ptr @FormatStr, ptr @2, i32 0)

; CHECK-LABEL: define i32 @main(i32 %argc, ptr %argv)
; CHECK-NEXT:  %1 = call i32 (ptr, ...) @printf(ptr @FormatStr, ptr @3, i32 2)

; CHECK:       declare i32 @printf(ptr, ...)
