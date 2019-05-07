CC = gcc
SRCS = main.c index.c buffer.c bbfs.c hash.c WCC.c stack.c SCC.c grail.c jsch.c graph.c
OBJS = $(SRCS:.c=.o)
OUT = exe

all: $(OBJS)
	$(CC) -o $(OUT) $(SRCS) -O2 -lpthread

.PHONY: clean

clean:
	rm -f $(OBJS) $(OUT)
