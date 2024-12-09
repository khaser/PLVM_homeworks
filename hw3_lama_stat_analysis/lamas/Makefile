CC=g++
NAME=lamas
CC_FLAGS=-m32 --std=c++20 -O2 -fstack-protector-all

all $(NAME): $(NAME).cpp
	$(CC) $(CC_FLAGS) -g -o $(NAME) $<

clean:
	$(RM) *.a *.o *~ $(NAME)
