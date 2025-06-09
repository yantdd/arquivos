CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
OBJ = parse_csv.o csv_to_bin.o imprime_bin.o busca_bin.o remove_reg_bin.o insere_reg_bin.o atualiza_reg_bin.o main.o
TARGET = programaTrab

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) 

parse_csv.o: parse_csv.c parse_csv.h
	$(CC) $(CFLAGS) -c parse_csv.c

csv_to_bin.o: csv_to_bin.c csv_to_bin.h parse_csv.h
	$(CC) $(CFLAGS) -c csv_to_bin.c
	
imprime_bin.o: imprime_bin.c imprime_bin.h
	$(CC) $(CFLAGS) -c imprime_bin.c

busca_bin.o: busca_bin.c busca_bin.h
	$(CC) $(CFLAGS) -c busca_bin.c

remove_reg_bin.o: remove_reg_bin.c remove_reg_bin.h
	$(CC) $(CFLAGS) -c remove_reg_bin.c

insere_reg_bin.o: insere_reg_bin.c insere_reg_bin.h
	$(CC) $(CFLAGS) -c insere_reg_bin.c

atualiza_reg_bin.o: atualiza_reg_bin.c atualiza_reg_bin.h
	$(CC) $(CFLAGS) -c atualiza_reg_bin.c

main.o: main.c parse_csv.h csv_to_bin.h imprime_bin.h busca_bin.h remove_reg_bin.h insere_reg_bin.h atualiza_reg_bin.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)

zip:
	zip -r projeto.zip *.c *.h Makefile