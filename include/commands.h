#ifndef COMMANDS_H
#define COMMANDS_H

// O nosso "processador" de comandos agora recebe argc e argv
void comandos_executar(int argc, char *argv[]);

// --- Definição da Estrutura de Comando ---
// A função agora deve aceitar argc e argv
typedef struct {
    const char *nome;
    void (*funcao)(int argc, char *argv[]);
    const char *descricao;
} Comando;


// --- Funções de Comando (Built-ins) ---
// (Estas são as funções que serão chamadas)

// Comando 'exit' 
void cmd_exit(int argc, char *argv[]);

// Comando 'pwd'
void cmd_pwd(int argc, char *argv[]);

#endif