NAME=multithread_memcpy
CC=g++
CC_FLAGS=-O2 -std=c++20 -g

all: test test-asan test-tsan

test: test.cpp
	$(CC) $(CC_FLAGS) test.cpp -o $@

test-asan: test.cpp
	$(CC) $(CC_FLAGS) -fsanitize=address test.cpp -o $@

test-tsan: test.cpp
	$(CC) $(CC_FLAGS) -fsanitize=thread test.cpp -o $@

clean:
	rm test
