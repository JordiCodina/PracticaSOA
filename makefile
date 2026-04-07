CC = gcc
CFLAGS = -g
EXTRAFLAGS = -Wall -Wextra
TARGET = fsutils
SRCS = main.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(EXTRAFLAGS)

valA: $(TARGET)
	valgrind --dsymutil=yes --track-origins=yes --leak-check=full --track-fds=yes --show-reachable=yes --show-leak-kinds=all -s ./$(TARGET)

a: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)