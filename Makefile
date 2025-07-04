CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
OBJ = parse_csv.o csv_to_bin.o imprime_bin.o busca_bin.o remove_reg_bin.o insere_reg_bin.o atualiza_reg_bin.o arvore_b.o cria_indice.o main.o busca_multipla.o insere_com_indice.o atualiza_com_indice.o
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

arvore_b.o: arvore_b.c arvore_b.h
	$(CC) $(CFLAGS) -c arvore_b.c

cria_indice.o: cria_indice.c cria_indice.h csv_to_bin.h arvore_b.h
	$(CC) $(CFLAGS) -c cria_indice.c

main.o: main.c parse_csv.h csv_to_bin.h imprime_bin.h busca_bin.h remove_reg_bin.h insere_reg_bin.h atualiza_reg_bin.h arvore_b.h cria_indice.h busca_multipla.h insere_com_indice.h
	$(CC) $(CFLAGS) -c main.c

busca_multipla.o: busca_multipla.c busca_multipla.h
	$(CC) $(CFLAGS) -c busca_multipla.c

insere_com_indice.o: insere_com_indice.c insere_com_indice.h
	$(CC) $(CFLAGS) -c insere_com_indice.c

atualiza_com_indice.o: atualiza_com_indice.c atualiza_com_indice.h
	$(CC) $(CFLAGS) -c atualiza_com_indice.c

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)

zip:
	zip -r projeto.zip *.c *.h Makefile