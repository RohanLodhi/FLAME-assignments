CC = gcc
CFLAGS = -Wall

TARGET = my_cmd
SRC = my_cmd.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
