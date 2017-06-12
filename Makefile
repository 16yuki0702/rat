CC		= gcc
CLFAGS		= -Wall -O4
PROG		= rat
SOURCES		= http.c server.c config.c main.c
OBJS		= $(SOURCES:.c=.o)

.PHONY: all

all: $(SOURECES) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
