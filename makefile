CC = gcc
CFLAGS = -g -Wall -Wextra -MMD
TARGET = fsutils

SRCS = main.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)