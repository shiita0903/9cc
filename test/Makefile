CC = gcc
CFLAGS = -std=c11 -g -static -no-pie

run:	test
	@echo "\n========== start test ==========\n"
	@./test
	@echo "\n==========  stop test ==========\n"

test:	test.s
	$(CC) $(CFLAGS) -o test test.s test_util.c $(LDFLAGS)

test.s:	../9cc test.c
	../9cc test.c > test.s

clean:
	rm test test.s

.PHONY: test clean