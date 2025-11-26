# Nome do programa
TARGET = miniShell

# Compilador e Flags
CC = gcc
CFLAGS = -Wall -g -Iinclude

# Estrutura
SRCS = $(wildcard src/*.c)
OBJ_DIR = obj
OBJS = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Diretório de Sandbox (Ambiente de Teste)
SANDBOX_DIR = sandbox

# Regra Principal
all: $(TARGET)

# Cria o executável
$(TARGET): $(OBJS)
	@echo "Ligando o programa..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "✅ Build completo!"

# Compila objetos
$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- REGRAS ESPECIAIS ---

# Cria um ambiente de teste limpo
env:
	@echo "Criando ambiente de teste em './$(SANDBOX_DIR)'..."
	@mkdir -p $(SANDBOX_DIR)
	@mkdir -p $(SANDBOX_DIR)/pasta1
	@mkdir -p $(SANDBOX_DIR)/pasta2
	@echo "Texto de teste" > $(SANDBOX_DIR)/arquivo1.txt
	@echo "Outro arquivo" > $(SANDBOX_DIR)/arquivo2.txt
	@echo "Ambiente pronto! Execute: ./$(TARGET) e depois 'cd $(SANDBOX_DIR)'"

# Executa o programa (cria o ambiente antes se não existir)
run: all env
	./$(TARGET)

# Limpeza Total (Binários + Sandbox)
clean:
	@echo "Limpando binários..."
	rm -f $(TARGET)
	rm -rf $(OBJ_DIR)
	@echo "Removendo ambiente de teste (sandbox)..."
	rm -rf sandbox
	@echo "✨ Limpeza concluída!"