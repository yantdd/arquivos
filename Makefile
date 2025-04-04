CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
OBJ = parse_csv.o csv_to_bin.o example.o
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

parse_csv.o: parse_csv.c parse_csv.h
	$(CC) $(CFLAGS) -c parse_csv.c

csv_to_bin.o: csv_to_bin.c csv_to_bin.h parse_csv.h
	$(CC) $(CFLAGS) -c csv_to_bin.c

example.o: example.c parse_csv.h csv_to_bin.h
	$(CC) $(CFLAGS) -c example.c

clean:
	rm -f $(OBJ) $(TARGET) *.bin

run: all
	./$(TARGET)