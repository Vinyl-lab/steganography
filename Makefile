CC = gcc
CFLAGS = -Wall -Iinclude

SRC = src/test_encode.c \
	  src/decode.c \
	  src/encode.c

TARGET = steganography

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)