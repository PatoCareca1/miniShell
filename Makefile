TARGET = miniShell

# Compilador
CC = gcc
# Flags de compilação:
# -Wall (mostra todos os avisos)
# -g (inclui informação de debug)
# -Iinclude (diz ao GCC para procurar ficheiros .h na pasta 'include')
CFLAGS = -Wall -g -Iinclude

# --- Compilação Automática ---

# Encontra todos os ficheiros .c na pasta src/
SRCS = $(wildcard src/*.c)

# Define o nome dos ficheiros objeto (compilados) na pasta obj/
# (Substitui 'src/' por 'obj/' e '.c' por '.o')
OBJ_DIR = obj
OBJS = $(patsubstitst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# O 'alvo' principal: o que acontece quando escreves 'make'
all: $(TARGET)

# Regra para criar o programa final (linkar os objetos)
$(TARGET): $(OBJS)
	@echo "Ligando o programa..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "Programa '$(TARGET)' criado com sucesso!"

# Regra para compilar os ficheiros .c para .o
# (Cria o diretório 'obj' se não existir)
$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar (apagar o programa e os objetos)
clean:
	@echo "Limpando..."
	rm -f $(TARGET)
	rm -rf $(OBJ_DIR)

# Regra para executar o programa
run: all
	./$(TARGET)