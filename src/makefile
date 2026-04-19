# Nome do executavel
TARGET = ted

# Nome do aluno
ALUNO = vitorms

# Compilador
CC = gcc

# Flags de compilacao
CFLAGS = -std=c99 -fstack-protector-all -Wall -Wextra -O0 -g -ggdb

# Diretorios
SRC_DIR   = .
INC_DIR   = ../include
UNITY_DIR = ../unity
LIB_DIR   = ./lib

# Flags
INC_FLAGS = -I$(INC_DIR)
LIB_FLAGS = -L$(LIB_DIR)
LIBS      = -lm
UNITY_OBJ = $(UNITY_DIR)/unity.c

# Fontes do executavel principal (sem os arquivos de teste)
MAIN_SRCS = main.c geo.c hashFile.c pessoa.c qry.c quadra.c svg.c arquivoPm.c
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

all: $(TARGET)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

$(TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) $(MAIN_OBJS) $(LIB_FLAGS) $(LIBS) -o $(TARGET)

testePessoa: testePessoa.c pessoa.o
	$(CC) $(CFLAGS) $(INC_FLAGS) -I$(UNITY_DIR) $^ $(UNITY_OBJ) $(LIBS) -o $@
	./$@

testeQuadra: testeQuadra.c quadra.o
	$(CC) $(CFLAGS) $(INC_FLAGS) -I$(UNITY_DIR) $^ $(UNITY_OBJ) $(LIBS) -o $@
	./$@

testeHashFile: testeHashFile.c hashFile.o
	$(CC) $(CFLAGS) $(INC_FLAGS) -I$(UNITY_DIR) $^ $(UNITY_OBJ) $(LIBS) -o $@
	./$@

testeGeo: testeGeo.c geo.o quadra.o hashFile.o svg.o
	$(CC) $(CFLAGS) $(INC_FLAGS) -I$(UNITY_DIR) $^ $(UNITY_OBJ) $(LIBS) -o $@
	./$@

testeQry: testeQry.c qry.o pessoa.o quadra.o hashFile.o svg.o
	$(CC) $(CFLAGS) $(INC_FLAGS) -I$(UNITY_DIR) $^ $(UNITY_OBJ) $(LIBS) -o $@
	./$@

tests: testePessoa testeQuadra testeHashFile testeGeo testeQry

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)
	rm -f testePessoa testeQuadra testeHashFile testeGeo testeQry
	rm -f *.dir pessoas.txt quadras.txt

pack: $(TARGET)
	rm -f ../$(ALUNO).zip
	echo $(ALUNO)
	date >> .entrega
	cd ..; zip -r $(ALUNO).zip $(SRC_DIR)/*.c $(SRC_DIR)/*.h ../include/*.h Makefile LEIA-ME.txt .entrega
	rm .entrega

.PHONY: all clean pack tests testePessoa testeQuadra testeHashFile testeGeo testeQry
