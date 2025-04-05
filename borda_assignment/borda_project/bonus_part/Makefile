CC=gcc
CFLAGS=-Wall -pthread

all: rtos_bonus slow_consumer

rtos_bonus: rtos_bonus.c circular_buffer.c
	$(CC) $(CFLAGS) -o rtos_bonus rtos_bonus.c circular_buffer.c

slow_consumer: slow_consumer.c circular_buffer.c
	$(CC) $(CFLAGS) -o slow_consumer slow_consumer.c circular_buffer.c

clean:
	rm -f rtos_bonus slow_consumer buffer_overflow.log
