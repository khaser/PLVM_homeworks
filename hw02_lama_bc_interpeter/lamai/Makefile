NAME=lamai
CC_FLAGS=-m32 -O2 -fstack-protector-all
LD_FLAGS=$(CC_FLAGS) -Wl,--no-warn-search-mismatch

all $(NAME): $(NAME).o
	$(CC) $(LD_FLAGS) -g -o $(NAME) $(NAME).o ../runtime/runtime.a

$(NAME).o: $(NAME).c
	$(CC) $(CC_FLAGS) -g -c $<

clean:
	$(RM) *.a *.o *~ $(NAME)
