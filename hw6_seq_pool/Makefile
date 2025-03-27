NAME=seq_pool
CC=g++
CC_FLAGS=-O2 -fstack-protector-all
LD_FLAGS=$(CC_FLAGS)

test: $(NAME).o test.o
	$(CC) $(LD_FLAGS) -g -o $@ $(NAME).o test.o

test.o: test.cpp
	$(CC) $(CC_FLAGS) -g -c $<

$(NAME).o: $(NAME).cpp
	$(CC) $(CC_FLAGS) -g -c $<

clean:
	$(RM) *.o $(NAME) test
