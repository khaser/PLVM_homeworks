EXECUTABLE = src/lamac
INSTALL ?= install -v
MKDIR ?= mkdir

.PHONY: all regression

all:
	$(MAKE) -C src
	$(MAKE) -C runtime
	$(MAKE) -C stdlib
	$(MAKE) -C runtime unit_tests.o
	$(MAKE) -C runtime invariants_check.o
	$(MAKE) -C runtime invariants_check_debug_print.o

regression:
	$(MAKE) clean check -j8 -C regression

STD_FILES=$(shell ls stdlib/*.[oi] stdlib/*.lama runtime/runtime.a runtime/Std.i)

install: all
	$(INSTALL) $(EXECUTABLE) `opam var bin`
	$(MKDIR) -p `opam var share`/Lama
	$(INSTALL) $(STD_FILES) `opam var share`/Lama/

uninstall:
	$(RM) -r `opam var share`/Lama
	$(RM) `opam var bin`/$(EXECUTABLE)

unit_tests:
	./runtime/unit_tests.o
	./runtime/invariants_check.o
	./runtime/invariants_check_debug_print.o

negative_scenarios_tests:
	$(MAKE) -C runtime negative_tests

performance:
	$(MAKE) -C performance check
.PHONY: performance

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C runtime
	$(MAKE) clean -C stdlib
	$(MAKE) clean -C regression
	$(MAKE) clean -C byterun
	$(MAKE) clean -C bench
