CC = gcc
CFLAGS = -std=c11 -g -static
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) $(CFLAGS) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	cd ./test; make

clean:
	rm -f 9cc *.o *~ tmp*
	cd ./test; make clean

.PHONY: test clean