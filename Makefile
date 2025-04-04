CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
OBJ = parse_csv.o csv_to_bin.o imprime_bin.o
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

parse_csv.o: parse_csv.c parse_csv.h
	$(CC) $(CFLAGS) -c parse_csv.c

csv_to_bin.o: csv_to_bin.c csv_to_bin.h parse_csv.h
	$(CC) $(CFLAGS) -c csv_to_bin.c
	
imprime_bin.o: imprime_bin.c imprime_bin.h
	$(CC) $(CFLAGS) -c imprime_bin.c

clean:
	rm -f $(OBJ) $(TARGET) 

run: all
	./$(TARGET)