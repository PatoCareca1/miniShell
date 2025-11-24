#include <stdio.h>
#include <string.h> // Para strtok() e strcspn()
#include "ui.h"
#include "commands.h"

#define MAX_LINHA 1024 // Tamanho máximo da linha de comando
#define MAX_ARGS 100   // Número máximo de argumentos (palavras)

/**
 * @brief "Quebra" a linha de input em argumentos (argv) usando strtok.
 * 
 * @param linha A linha de input lida com fgets().
 * @param argv O array (vetor de strings) para guardar as palavras.
 * @return O número de argumentos (argc).
 */
int parsear_linha(char *linha, char *argv[]) {
    int argc = 0;
    // Define os delimitadores: espaço, tabulação e nova linha
    const char *delimitadores = " \t\n"; 
    
    // strtok "corta" a string e retorna o primeiro "token"
    char *token = strtok(linha, delimitadores);
    
    while (token != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = token; // Guarda o ponteiro para a palavra
        token = strtok(NULL, delimitadores); // Pede o próximo token
    }
    
    argv[argc] = NULL; // O vetor argv deve terminar com NULL
    return argc;
}

int main(void) {
    char linha[MAX_LINHA];          // Buffer para a linha inteira
    char *meu_argv[MAX_ARGS];     // O nosso vetor de argumentos
    int meu_argc;
    
    // 1. Opcional: Mostrar a tela de boas-vindas
    ui_mostrar_boas_vindas();
    
    // 2. Loop principal do shell
    while (1) {
        // 2.1. Afixa o prompt
        printf("mini-shell> ");
        
        // 2.2. Lê a linha
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            // Se fgets falhar (ex: Ctrl+D), trata como 'exit'
            printf("\n");
            cmd_exit(0, NULL); // Sai de forma limpa
        }
        
        // 2.3. Interpreta a linha (Parser)
        meu_argc = parsear_linha(linha, meu_argv);
        
        // 2.4. Executa o comando
        comandos_executar(meu_argc, meu_argv);
    }
    
    return 0; // Nunca deve chegar aqui
}