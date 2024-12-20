CC=g++
NAME=lamav
CC_FLAGS=-m32 --std=c++20 -g -O2 -fstack-protector-all -D_GLIBCXX_DEBUG

all $(NAME): $(NAME).cpp util.cpp
	$(CC) $(CC_FLAGS) -o $(NAME) $^

clean:
	$(RM) *.a *.o *~ $(NAME)
