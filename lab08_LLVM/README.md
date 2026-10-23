# Laboratory 08: LLVM introduction

The solution implements integer AST nodes, parenthesized AST printing, and LLVM
IR generation for binary addition and subtraction. `main` constructs
`((10 + 20) - 5)` by hand. It prints the expression to standard output and the
verified LLVM function to standard error.

## Build and run

Install LLVM 17, Clang, Flex, and Make. On Ubuntu:

```sh
sudo apt install llvm-17 clang-17 flex make
make
```

For an LLVM installation outside PATH, specify its configuration executable:

```sh
make LLVM_CONFIG=/path/to/llvm-config CXX=clang++
```

`make build` only builds; `make run` runs the example; `make clean` removes
 generated files. The lexer is retained from the starter project and can tokenize
numbers and addition/subtraction expressions, but the lab's manually built AST
does not call it. There is no interactive parser or JIT execution.

## Expected output

```text
((10 + 20) - 5)
define internal i32 @__anon_expr() {
entry:
  %addtmp = add i32 10, 20
  %subtmp = sub i32 %addtmp, 5
  ret i32 %subtmp
}
```

`IRBuilder<NoFolder>` preserves the arithmetic instructions instead of folding
them to a constant. Number nodes generate signed 32-bit integer constants.
An invalid operator or missing operand fails code generation; each successfully
generated function is checked with LLVM's verifier before printing.
