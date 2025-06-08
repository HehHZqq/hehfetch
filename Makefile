CC = gcc
CFLAGS = -Wall -Wextra
TARGET = hehfetch

all: $(TARGET)

$(TARGET): hehfetch.c
	$(CC) $(CFLAGS) -o $(TARGET) hehfetch.c

clean:
	rm -f $(TARGET)

.PHONY: all clean 