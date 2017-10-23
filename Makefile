CC		= gcc
CFLAGS		= -Wall -O4
LDFLAGS		+= -lhiredis -L/usr/local/lib
INCLUDE		+= -I/usr/local/include/hiredis
PROG		= rat
SOURCES		= rat_log.c rat_string.c rat_http.c rat_mqtt.c rat_backend.c rat_server.c rat_config.c rat.c
OBJS		= $(SOURCES:.c=.o)

.PHONY: all

all: $(SOURECES) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

clean:
	rm -rf *.o rat
