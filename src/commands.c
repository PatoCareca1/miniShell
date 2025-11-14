#include <stdio.h>
#include <string.h> // Para strcmp (comparar strings)
#include <stdlib.h> // Para exit()
#include "commands.h"

// --- IMPLEMENTAÇÃO DAS FUNÇÕES DOS COMANDOS ---
// (Estas são as funções que realmente fazem o trabalho)

void cmd_ajuda(void) {
    printf("Comandos disponiveis:\n");
    
    // (Isto é avançado, mas idealmente esta função
    // iria ler o mapa de comandos e imprimir a descrição de cada um)
    printf(" - ajuda : Mostra esta mensagem de ajuda\n");
    printf(" - sair  : Sai do programa\n");
    // Quando adicionares um novo comando, adiciona-o aqui também
}

void cmd_sair(void) {
    printf("Ate logo!\n");
    exit(0); // Termina o programa com sucesso
}


// --- O MAPA DE COMANDOS ---
// Aqui é onde a escalabilidade acontece!
// Para adicionar um comando, basta adicionar uma linha.
static const Comando mapa_de_comandos[] = {
    { "ajuda", cmd_ajuda, "Mostra a mensagem de ajuda" },
    { "sair",  cmd_sair,  "Sai do programa" },
    // { "listar", cmd_listar_utilizadores, "Lista todos os utilizadores" },
    // (Descomenta a linha acima quando criares a função)
};

// --- O MOTOR DE EXECUÇÃO ---

void comandos_executar(const char *input) {
    // Itera sobre o mapa de comandos
    int num_comandos = sizeof(mapa_de_comandos) / sizeof(Comando);
    
    for (int i = 0; i < num_comandos; i++) {
        // Compara o input do utilizador com o nome do comando no mapa
        if (strcmp(input, mapa_de_comandos[i].nome) == 0) {
            
            // Se encontrou, executa a função associada (ponteiro de função)
            mapa_de_comandos[i].funcao();
            return; // Encontrou e executou, podemos parar
        }
    }
    
    // Se o loop terminou e não encontrou nada
    printf("Comando desconhecido: '%s'. Digite 'ajuda'.\n", input);
}