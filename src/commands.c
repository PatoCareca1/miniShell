#include <stdio.h>
#include <stdlib.h> // Para exit()
#include <string.h> // Para strcmp()
#include <unistd.h> // Para getcwd()
#include "commands.h"

// --- Implementação das Funções de Comando ---

/**
 * @brief Sai do mini-shell.
 * [cite: 13, 21]
 */
void cmd_exit(int argc, char *argv[]) {
    printf("Ate logo!\n");
    exit(0); // Termina o programa com sucesso
}

/**
 * @brief Mostra o diretório de trabalho atual.
 * Implementa o comando 'pwd' usando a chamada de sistema getcwd().
 * [cite: 21, 42]
 */
void cmd_pwd(int argc, char *argv[]) {
    char buffer[1024]; // Buffer para guardar o caminho
    
    // getcwd() é a chamada de sistema/função C que faz o trabalho
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        printf("%s\n", buffer);
    } else {
        // Se getcwd() falhar, usamos perror() [cite: 24, 44]
        perror("Erro ao obter o diretorio atual");
    }
}


// --- O MAPA DE COMANDOS (A nossa tabela) ---
// Para adicionar um comando, basta adicionar uma linha.
static const Comando mapa_de_comandos[] = {
    { "exit", cmd_exit, "Sai do mini-shell" },
    { "pwd",  cmd_pwd,  "Mostra o diretorio de trabalho atual" },
    
    // { "mkdir", cmd_mkdir, "Cria um novo diretorio" },
    // { "ls",    cmd_ls,    "Lista ficheiros e diretorios" },
};
static const int NUM_COMANDOS = sizeof(mapa_de_comandos) / sizeof(Comando);


// --- O MOTOR DE EXECUÇÃO ---

/**
 * @brief Procura e executa um comando com base no argv[0].
 */
void comandos_executar(int argc, char *argv[]) {
    // Se nenhum comando foi digitado (linha em branco), ignora. 
    if (argc == 0) {
        return;
    }
    
    const char *nome_comando = argv[0];
    
    // Itera sobre o mapa de comandos
    for (int i = 0; i < NUM_COMANDOS; i++) {
        if (strcmp(nome_comando, mapa_de_comandos[i].nome) == 0) {
            // Encontrou! Executa a função associada,
            // passando os argumentos
            mapa_de_comandos[i].funcao(argc, argv);
            return; // Encontrou e executou, podemos parar
        }
    }
    
    // Se o loop terminou, o comando não foi encontrado. [cite: 25]
    printf("Comando desconhecido: '%s'.\n", nome_comando);
}