; RUN: opt -load-pass-plugin %lib/libDynamicCallCounter%ext \
; RUN:   -passes="dynamic-cc,verify" -S %s 2>&1 | FileCheck %s

; CHECK:      @CounterFor_foo = common global i32 0, align 4
; CHECK-NEXT: @CounterResultHeader = private global [182 x i8]
; CHECK-NEXT: @CounterResultFormater = private global [14 x i8]
; CHECK-NEXT: @0 = private unnamed_addr constant [4 x i8] c"foo\00", align 1
; CHECK-NEXT: @llvm.global_dtors = appending global
; CHECK-SAME: @printf_wrapper

define void @foo() {
; CHECK-LABEL: define void @foo()
; CHECK-NEXT:  %1 = load i32, ptr @CounterFor_foo, align 4
; CHECK-NEXT:  %2 = add i32 %1, 1
; CHECK-NEXT:  store i32 %2, ptr @CounterFor_foo, align 4
; CHECK-NEXT:  ret void
  ret void
}

; CHECK-LABEL: define private void @printf_wrapper()
; CHECK-NEXT:  %1 = call i32 (ptr, ...) @printf(ptr @CounterResultHeader)
; CHECK-NEXT:  %2 = load i32, ptr @CounterFor_foo, align 4
; CHECK-NEXT:  %3 = call i32 (ptr, ...) @printf(ptr @CounterResultFormater, ptr @0, i32 %2)
; CHECK-NEXT:  ret void
