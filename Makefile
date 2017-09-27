CC		= gcc
CLFAGS		= -Wall -O4
PROG		= rat
SOURCES		= rat_log.c rat_string.c rat_http.c rat_mqtt.c rat_server.c rat_config.c rat.c
OBJS		= $(SOURCES:.c=.o)

.PHONY: all

all: $(SOURECES) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o rat
