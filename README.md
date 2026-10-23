# Translators Design — Compilers in C/C++

Flex scanners, recursive-descent parsers, ASTs, stack-machine code generation, and introductory LLVM IR.

```text
source → tokens → parser → AST → generated instructions
```

This repository collects my laboratory exercises, seminar work, and ongoing project for the **Translators Design** course taught by **Emil Chifu** at the **Technical University of Cluj-Napoca (UTCN)**.

The work so far explores lexical analysis with Flex, recursive-descent parsing, abstract syntax trees (ASTs), expression evaluation, and code generation in C and C++.

## Repository contents

### `AST_TODO/` — Regular-expression parser and AST

This exercise implements a Flex scanner and a recursive-descent parser for regular expressions containing letters, grouping, alternation (`|`), implicit concatenation, and the Kleene star (`*`).

The parser builds an AST with letter leaves and operator nodes. Concatenation is represented explicitly by `&` in the tree, although it has no visible symbol in the input. Alternation and concatenation use right recursion, and the resulting AST is printed as a top-down ASCII tree.

Example input:

```text
a(b|c)*
```

Build and run:

```sh
cd AST_TODO
make
```

Enter an expression followed by a newline. End input with EOF (Ctrl+D on Linux/macOS).

### `eBNF TODO/` — Integer-expression evaluator

This exercise translates an eBNF repetition into a parsing loop. A Flex scanner recognizes integers and punctuation, while the recursive-descent parser evaluates addition and parenthesized expressions directly.

The implemented grammar is:

```text
E ::= T ('+' T)*
T ::= integer | '(' E ')'
```

Each nonempty input line contains one expression. The evaluator checks for unexpected trailing tokens and missing operands or closing parentheses. It also accepts a final line without a newline.

Build and run:

```sh
cd "eBNF TODO"
make
```

For example, `2 + (3 + 4)` prints `9`.

### `ProjectTD/` — Simple compiler in progress

The current project combines a Flex scanner, a handwritten recursive-descent parser, AST construction, and a backend that emits textual instructions for a stack machine.

The parser currently supports integer constants, addition, parentheses, and `while (condition) body` expressions. Addition builds a left-associated AST. The backend traverses the AST and emits instructions such as `push`, `add`, conditional jumps, and labels.

The backend also contains cases for assignments, conditionals, printing, and other arithmetic and comparison operators. These are groundwork for further development: the current scanner and parser do not yet expose the full set of backend operations.

Build and run from the repository root:

```sh
cd ProjectTD
flex -o lex.yy.c simple.l
gcc simple.c compiler.c lex.yy.c -o SIMPLE -ll
./SIMPLE
```

On Linux, use `-lfl` instead of `-ll` if required by the Flex installation.

For example, input `1 + 2` produces:

```text
    push    1
    push    2
    add
```

### `lab08_LLVM/` — Introduction to LLVM IR generation

This laboratory introduces C++ AST classes and LLVM's intermediate representation. Number nodes generate signed 32-bit integer constants, while binary-expression nodes generate addition and subtraction instructions.

The example manually constructs the AST for `((10 + 20) - 5)`, prints its parenthesized representation, and generates an anonymous LLVM function returning the result. The generated function is checked with LLVM's verifier before being printed. Constant folding is disabled in the builder so that the arithmetic instructions remain visible.

The included lexer is retained from the starter code; it is not called by the manually constructed AST example. Interactive parsing and JIT execution are not implemented.

Build and run:

```sh
cd lab08_LLVM
make
```

If LLVM is installed outside `PATH`:

```sh
make LLVM_CONFIG=/path/to/llvm-config CXX=clang++
```

See the [laboratory README](lab08_LLVM/README.md) for dependencies, expected output, and additional build targets.

## Requirements

- A C compiler for the parsing exercises and project.
- Flex for generating lexical analyzers.
- Make for the laboratory build commands.
- LLVM 17 development tools and a compatible C++ compiler for `lab08_LLVM/`.

## Progress so far

- Implemented token recognition with Flex.
- Used one-token lookahead in recursive-descent parsers.
- Translated grammar alternatives, recursion, and repetition into C functions and loops.
- Built and displayed ASTs for regular expressions.
- Evaluated integer addition expressions with parentheses.
- Generated stack-machine instructions from a simple language AST.
- Generated and verified LLVM IR from a manually constructed C++ AST.

This repository will grow as I complete further laboratories, seminars, and project work. Folder names containing `TODO` come from the exercise structure; the descriptions above reflect the current implementations.
