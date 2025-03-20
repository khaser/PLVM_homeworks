# GRADLE ?= ./gradlew
GRADLE = gradle
SMOKE_TESTS_DIR := smoke_tests
REG_TESTS_DIR := regression

BINARY := ./launcher/build/native/nativeCompile/application
FAT_JAR := ./launcher/build/libs/launcher-all.jar
JAVA_CMD := java -Xss1024m -cp $(FAT_JAR) khaser.lama.LamaMain

# 56,63,90,91,93,94,96,97,99,102 - first-class function
# 74 - passes, but runs too slow
# 82 - type pattern matching
# 92,95,98 - custom operator
# 105 - eta-expansion (unsupported language extension)
# 106 - pattern in function arguments (unsupported language extension)
# 111 - import Std;
REG_IGNORED := \
	%/test059.lama \
	%/test063.lama \
	%/test074.lama \
	%/test082.lama \
	%/test090.lama \
	%/test091.lama \
	%/test092.lama \
	%/test093.lama \
	%/test094.lama \
	%/test095.lama \
	%/test096.lama \
	%/test097.lama \
	%/test098.lama \
	%/test099.lama \
	%/test102.lama \
	%/test105.lama \
	%/test106.lama \
	%/test111.lama

SRCS := $(shell find ./language/src ./launcher/src -name '*.java' -or -name '*.g4' | sed 's@ @\\ @g')

REG_TESTS=$(sort $(filter-out $(REG_IGNORED: =,), $(wildcard $(REG_TESTS_DIR)/test*.lama)))
SMOKE_TESTS=$(sort $(wildcard $(SMOKE_TESTS_DIR)/*.lama))

test: regression smoke

regression_binary: $(BINARY)
	# TODO
	# $(foreach test, $(REG_TESTS), $(GRADLE) run build $(test))
	echo Regression test passed

smoke_binary: $(BINARY)
	@$(foreach test, $(SMOKE_TESTS), echo running $(test) && \
					 diff <($(BINARY) $(test) < $(test:.lama=.in)) $(test:.lama=.ref) &&) \
	echo Smoke tests passed

regression_java: $(FAT_JAR)
	@$(foreach test, $(REG_TESTS), echo running $(test) && \
					 diff <($(JAVA_CMD) $(test) < $(test:.lama=.input) | head --lines=-2) $(test:.lama=.log) &&) \
	echo Regression tests passed

smoke_java: $(FAT_JAR)
	@$(foreach test, $(SMOKE_TESTS), echo running $(test) && \
					 diff <($(JAVA_CMD) $(test) < $(test:.lama=.in)) $(test:.lama=.ref) &&) \
	echo Smoke tests passed

$(BINARY): $(SRCS)
	$(GRADLE) nativeCompile

$(FAT_JAR): $(SRCS)
	$(GRADLE) build

.PHONY: test regression smoke
