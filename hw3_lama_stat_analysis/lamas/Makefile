CC=g++
NAME=lamas
CC_FLAGS=-m32 --std=c++20 -g -O2 -fstack-protector-all

all $(NAME): $(NAME).cpp
	$(CC) $(CC_FLAGS) -o $(NAME) $<

clean:
	$(RM) *.a *.o *~ $(NAME)
