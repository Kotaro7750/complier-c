CFLAGS=-Wall -g -std=c11 -static

cclemon: cclemon.c

test: cclemon
	./test.sh

clean:
	rm -f cclemon *.o *~ tmp*

.PHONY: test clean
