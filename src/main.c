#include <stdio.h>
#include "ui.h"         // Inclui o módulo de UI
#include "commands.h"   // Inclui o módulo de comandos

#define MAX_INPUT 100 // Tamanho máximo do buffer de input

// Função para ler o input do utilizador de forma segura
void ler_input(char *buffer, int tamanho) {
    printf("> "); // O "prompt"
    fgets(buffer, tamanho, stdin);
    
    // Remove o '\n' (nova linha) que o fgets adiciona
    buffer[strcspn(buffer, "\n")] = 0;
}

int main(void) {
    char input_buffer[MAX_INPUT];
    
    // 1. O módulo UI é responsável por dar as boas-vindas
    ui_mostrar_boas_vindas();
    
    // 2. Loop principal do programa
    while (1) {
        // 3. Lê o input
        ler_input(input_buffer, MAX_INPUT);
        
        // 4. O módulo de Comandos é responsável por tratar o input
        comandos_executar(input_buffer);
    }
    
    return 0; // (Nunca chega aqui, saímos com 'cmd_sair')
}