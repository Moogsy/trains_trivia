# Variables
CC = gcc
CFLAGS = -Wall -g
LIBS = -lsqlite3
TARGET = client
SRC = main.c yyjson.c utils.c

# Rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

