define i32 @main() {
entry:
  %multmp = mul i32 3, 4
  %addtmp = add i32 2, %multmp
  ret i32 %addtmp
}
