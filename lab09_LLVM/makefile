LLVM_CONFIG ?= $(shell command -v llvm-config-17 2>/dev/null || command -v llvm-config 2>/dev/null || echo /opt/homebrew/opt/llvm@17/bin/llvm-config)
LLVM_BINDIR := $(shell $(LLVM_CONFIG) --bindir)
ifeq ($(shell uname -s),Darwin)
CXX := /usr/bin/clang++
else
CXX := $(LLVM_BINDIR)/clang++
endif
LEX = flex

.PHONY: build_and_run clean
build_and_run: main

main: main.cpp lexer.cpp
	$(CXX) -g -O3 main.cpp lexer.cpp `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core` -o main

lexer.cpp: lexer.l
	$(LEX) -o $@ $<

clean:
	rm -f lexer.cpp main output.ll

# ./main && lli-17 output.ll; echo "Result is: $?"
