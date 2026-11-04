# Laboratory 08: LLVM introduction

Implemented integer AST nodes, parenthesized expression printing, and LLVM IR
for addition and subtraction. `main.cpp` builds `((10 + 20) - 5)` by hand.
LLVM verifies the generated function. `NoFolder` keeps arithmetic instructions visible.

## Run

On macOS, install LLVM 17 with `brew install llvm@17`, then run `make`.
The makefile detects the Apple Silicon Homebrew installation automatically.
For other installations:

```sh
make LLVM_CONFIG=/path/to/llvm-config CXX=/path/to/clang++
```

`make build` compiles, `make run` runs, and `make clean` removes generated files.

Expected output (expression on stdout, IR on stderr):

```text
((10 + 20) - 5)
define internal i32 @__anon_expr() {
entry:
  %addtmp = add i32 10, 20
  %subtmp = sub i32 %addtmp, 5
  ret i32 %subtmp
}
```

The lexer is retained from the starter project. The example constructs the AST
manually; there is no interactive parser or JIT execution.
