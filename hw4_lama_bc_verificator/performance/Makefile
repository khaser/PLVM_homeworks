TESTS=$(sort $(basename $(wildcard *.lama)))

LAMAC=../src/lamac
LAMAI=../lamai/lamai

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo "Upstream (recursive AST interpreter):" $@
	@LAMA=../runtime time -f "$@\t%U" $(LAMAC) -i $< < /dev/null
	@echo "Upstream (recursive bytecode interpreter):" $@
	@LAMA=../runtime time -f "$@\t%U" $(LAMAC) -s $< < /dev/null
	@echo "Lamai (iterative bytecode interpreter):" $@
	@LAMA=../runtime $(LAMAC) -b $<
	@time -f "$@\t%U" $(LAMAI) $@.bc
	@rm $@.bc

clean:
	$(RM) test*.log *.s *~ $(TESTS) *.i
