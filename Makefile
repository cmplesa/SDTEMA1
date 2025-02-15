CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = sfl

.PHONY: build run_sfl clean

build: $(TARGET)

$(TARGET): sfl.o
	$(CC) $(CFLAGS) -o $@ $^

sfl.o: sfl.c
	$(CC) $(CFLAGS) -c -o $@ $<

run_sfl: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o