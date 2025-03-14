# GRADLE ?= ./gradlew
GRADLE = gradle
SMOKE_TESTS_DIR := smoke_tests
REG_TESTS_DIR := regression

BINARY := ./launcher/build/native/nativeCompile/application
FAT_JAR := ./launcher/build/libs/launcher-all.jar
JAVA_CMD := java -cp $(FAT_JAR) khaser.lama.LamaMain

SRCS := $(shell find ./language/src ./launcher/src -name '*.java' | sed 's@ @\\ @g')

# TODO: add ignored tests list

REG_TESTS=$(sort $(wildcard $(REG_TESTS_DIR)/test*.lama))
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
	$(foreach test, $(REG_TESTS), echo running $(test) && \
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
