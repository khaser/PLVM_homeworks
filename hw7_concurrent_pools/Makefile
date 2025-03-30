CC=g++
CC_FLAGS=-std=c++20 -O2 -g -Wno-unused-result
LD_FLAGS=$(CC_FLAGS)

VARIANTS=malloc localPools lockFreePool mutexPool
HEADERS=$(wildcard *.h)
TEST_BINS=$(addprefix test_, $(VARIANTS))
TEST_OBJS=$(addsuffix .o, $(TEST_BINS))

test_all: build_all
	@$(foreach var,$(VARIANTS),echo Executing: ./test_$(var) && ./test_$(var) && echo && ) :

build_all: $(TEST_BINS)

$(TEST_BINS): pool_depletion_logger.o seq_pool.o lock_free_seq_pool.o $(TEST_OBJS)
	$(CC) $(LD_FLAGS) -g -o $@ pool_depletion_logger.o seq_pool.o lock_free_seq_pool.o $@.o

$(TEST_OBJS): test.cpp $(HEADERS)
	$(CC) -DVARIANT=$(patsubst test_%.o,%,$@) $(CC_FLAGS) -g -c $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CC_FLAGS) -g -c $< -o $@

clean:
	$(RM) *.o $(TEST_BINS)
