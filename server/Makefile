# Variables
CC = gcc
CFLAGS = -Wall -g
LIBS = -lsqlite3
TARGET = server
SRC = main.c yyjson.c questions.c

# Rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

