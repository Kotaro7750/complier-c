CFLAGS=-Wall -g -std=c11 -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

cclemon: $(OBJS)
	$(CC) -o cclemon $(OBJS) $(LDFLAGS)

$(OBJS): cclemon.h

test: cclemon
	./test.sh

clean:
	rm -f cclemon *.o *~ tmp*

.PHONY: test clean
