; RUN: opt -load-pass-plugin %lib/libPrintSomething%ext -passes=print-something \
; RUN:   -disable-output %s 2>&1 | FileCheck %s

; CHECK:      PrintSomething from foo, number of arguments: 1
; CHECK-NEXT: PrintSomething from bar, number of arguments: 2
; CHECK-NEXT: PrintSomething from fez, number of arguments: 3
; CHECK-NEXT: PrintSomething from main, number of arguments: 2

define i32 @foo(i32) {
  %2 = shl nsw i32 %0, 1
  ret i32 %2
}

define i32 @bar(i32, i32) {
  %3 = shl i32 %1, 1
  %4 = add nsw i32 %3, %0
  ret i32 %4
}

define i32 @fez(i32, i32, i32) {
  %4 = shl i32 %1, 1
  %5 = add nsw i32 %4, %0
  %6 = mul nsw i32 %2, 3
  %7 = add nsw i32 %5, %6
  ret i32 %7

}

define i32 @main(i32, ptr nocapture readnone) {
  %3 = tail call i32 @foo(i32 1)
  %4 = tail call i32 @bar(i32 1, i32 11)
  %5 = add nsw i32 %4, %3
  %6 = tail call i32 @fez(i32 1, i32 11, i32 111)
  %7 = add nsw i32 %5, %6
  ret i32 %7
}
