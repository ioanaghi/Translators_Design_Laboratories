LLVM_CONFIG ?= $(shell command -v llvm-config-17 2>/dev/null || command -v llvm-config 2>/dev/null)
ifeq ($(origin CXX), default)
CXX := $(shell command -v clang++-17 2>/dev/null || command -v clang++ 2>/dev/null)
endif
LEX := flex

.PHONY: build_and_run build run clean check-deps

build_and_run: run

build: main

check-deps:
	@test -n "$(LLVM_CONFIG)" || { echo "LLVM development tools missing: install LLVM 17 or set LLVM_CONFIG=/path/to/llvm-config" >&2; exit 1; }
	@command -v "$(LLVM_CONFIG)" >/dev/null || { echo "Cannot find $(LLVM_CONFIG)" >&2; exit 1; }

main: main.cpp lexer.cpp | check-deps
	$(CXX) -g -O3 $(CPPFLAGS) $(CXXFLAGS) main.cpp lexer.cpp $(shell $(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core 2>/dev/null) $(LDFLAGS) -o $@

lexer.cpp: lexer.l
	$(LEX) -o $@ $<

run: main
	./main

clean:
	rm -f lexer.cpp main
