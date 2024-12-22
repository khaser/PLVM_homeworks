CC=g++
NAME=lamav
CC_FLAGS=-m32 --std=c++20 -g -O2 -fstack-protector-all -D_GLIBCXX_DEBUG -Wno-write-strings
LD_FLAGS=$(CC_FLAGS) -Wl,--no-warn-search-mismatch

all $(NAME): $(NAME).o util.o
	$(CC) $(LD_FLAGS) -o $(NAME) $(NAME).o util.o ../runtime/runtime.a

$(NAME).o: $(NAME).cpp
	$(CC) $(CC_FLAGS) -c $<

util.o: util.cpp
	$(CC) $(CC_FLAGS) -c $<

clean:
	$(RM) *.a *.o *~ $(NAME)
