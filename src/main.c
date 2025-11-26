#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ui.h"
#include "commands.h"

#define MAX_LINHA 1024
#define MAX_ARGS 100

// Configuração do Sandbox
#define SANDBOX_DIR "sandbox"

// Função para iniciar o ambiente isolado
void inicializar_sandbox() {
    struct stat st = {0};

    // 1. Verifica se a pasta existe, se não, cria
    if (stat(SANDBOX_DIR, &st) == -1) {
        if (mkdir(SANDBOX_DIR, 0755) != 0) {
            perror("Erro ao criar sandbox");
            exit(1);
        }
    }

    // 2. Entra na pasta
    if (chdir(SANDBOX_DIR) != 0) {
        perror("Erro ao entrar no sandbox");
        exit(1);
    }
}

int parsear_linha(char *linha, char *argv[]) {
    int argc = 0;
    const char *delimitadores = " \t\n"; 
    
    char *token = strtok(linha, delimitadores);
    
    while (token != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, delimitadores);
    }
    
    argv[argc] = NULL;
    return argc;
}

int main(void) {
    char linha[MAX_LINHA];
    char *meu_argv[MAX_ARGS];
    int meu_argc;
    
    // --- ATIVAR MODO SANDBOX ---
    inicializar_sandbox();
    // ---------------------------

    ui_mostrar_boas_vindas();
    
    while (1) {
        printf("mini-shell> ");
        
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\n");
            cmd_exit(0, NULL);
        }
        
        meu_argc = parsear_linha(linha, meu_argv);
        comandos_executar(meu_argc, meu_argv);
    }
    
    return 0;
}