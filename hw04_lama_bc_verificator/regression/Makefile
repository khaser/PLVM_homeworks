DEBUG_FILES=stack-dump-before data-dump-before extra-roots-dump-before heap-dump-before stack-dump-after data-dump-after extra-roots-dump-after heap-dump-after
TESTS=$(sort $(filter-out test111, $(basename $(wildcard test*.lama))))

LAMAC=../src/lamac
LAMAV=../lamav/lamav

.PHONY: check $(TESTS)


check: $(TESTS)

$(TESTS): %: %.lama
	@echo "regression/$@"
	@# lamai testing
	@LAMA=../runtime $(LAMAC) -b $<
	@cat $@.input | $(LAMAV) $@.bc 2>/dev/null > $@.log && diff $@.log orig/$@.log
	@rm $@.bc
	@# Upstream ocaml compiler/interpreters testing
	@# @cat $@.input | LAMA=../runtime $(LAMAC) -i $< > $@.log && diff $@.log orig/$@.log
	@# @cat $@.input | LAMA=../runtime $(LAMAC) -ds -s $< > $@.log && diff $@.log orig/$@.log
	@# @LAMA=../runtime $(LAMAC) $< && cat $@.input | ./$@ > $@.log && diff $@.log orig/$@.log

ctest111:
	@echo "regression/test111"
	@LAMA=../runtime $(LAMAC) test111.lama && cat test111.input | ./test111 > test111.log && diff test111.log orig/test111.log

clean:
	$(RM) test*.log *.s *.sm *~ $(TESTS) *.i $(DEBUG_FILES) test111
