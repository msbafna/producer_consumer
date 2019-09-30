TARGET ?= a.out

CC = gcc
CFLAGS := -g -Wall -Wextra -Werror -Wstrict-prototypes -DDEBUG
IFLAGS := -I.
LFLAGS := -L. -lpthread

CFILES := main.c thread_lib.c producer_consumer.c
DEPS := common_defs.h thread_lib.h producer_consumer.h
OBJS := main.o thread_lib.o producer_consumer.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $^ $(LFLAGS)

all: $(TARGET)

clean:
	rm -f $(TARGET) *.o cscope.out tags

.PHONY: clean
