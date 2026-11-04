LLVM_CONFIG ?= $(shell command -v llvm-config-17 2>/dev/null || command -v llvm-config 2>/dev/null || { for p in /opt/homebrew/opt/llvm@17/bin/llvm-config /usr/local/opt/llvm@17/bin/llvm-config; do if test -x "$$p"; then echo "$$p"; break; fi; done; })
ifeq ($(origin CXX),default)
CXX := clang++
endif

.PHONY: build_and_run build run clean check-llvm

build_and_run: run

check-llvm:
	@test -n "$(LLVM_CONFIG)" && "$(LLVM_CONFIG)" --version >/dev/null 2>&1 || { echo 'LLVM not found. Install with brew install llvm@17 or set LLVM_CONFIG to the actual llvm-config path.'; exit 1; }

build: main

main: main.cpp lexer.cpp makefile | check-llvm
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -g -O3 main.cpp lexer.cpp `"$(LLVM_CONFIG)" --cxxflags --ldflags --system-libs --libs core` $(LDFLAGS) $(LDLIBS) -o $@ -ll

lexer.cpp: lexer.l
	lex -o $@ $<

run: build
	./main

clean:
	rm -f lexer.cpp main
