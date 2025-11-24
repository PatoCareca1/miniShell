#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // Para getcwd, chdir
#include <sys/stat.h>   // Para mkdir
#include <errno.h>      // Para códigos de erro extras
#include "commands.h"

// --- IMPLEMENTAÇÃO ---

void cmd_exit(int argc, char *argv[]) {
    printf("Ate logo!\n");
    exit(0);
}

void cmd_pwd(int argc, char *argv[]) {
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        printf("%s\n", buffer);
    } else {
        perror("mini-shell: pwd");
    }
}

/**
 * @brief Cria um novo diretório.
 * Uso: mkdir <nome_da_pasta>
 */
void cmd_mkdir(int argc, char *argv[]) {
    // 1. Validar se o utilizador digitou o nome da pasta
    if (argc < 2) {
        printf("Uso: mkdir <nome_do_diretorio>\n");
        return;
    }

    // 2. Tentar criar o diretório
    // 0755 é a permissão padrão (Leitura/Escrita para o dono, Leitura para outros)
    if (mkdir(argv[1], 0755) == 0) {
        printf("Diretorio '%s' criado com sucesso.\n", argv[1]);
    } else {
        // Se falhar (ex: pasta já existe), perror explica o porquê
        perror("mini-shell: mkdir");
    }
}

/**
 * @brief Muda o diretório atual.
 * Uso: cd <caminho>
 */
void cmd_cd(int argc, char *argv[]) {
    char *caminho;

    if (argc < 2) {
        caminho = getenv("HOME");
        if (caminho == NULL) {
            printf("mini-shell: cd: Variavel HOME nao definida\n");
            return;
        }
    } else {
        caminho = argv[1];
    }

    if (chdir(caminho) == 0) {
    } else {
        perror("mini-shell: cd");
    }
}


// --- MAPA DE COMANDOS ---

static const Comando mapa_de_comandos[] = {
    { "exit",  cmd_exit,  "Sai do mini-shell" },
    { "pwd",   cmd_pwd,   "Mostra o diretorio atual" },
    { "mkdir", cmd_mkdir, "Cria um diretorio" },
    { "cd",    cmd_cd,    "Muda de diretorio" },
};

static const int NUM_COMANDOS = sizeof(mapa_de_comandos) / sizeof(Comando);

void comandos_executar(int argc, char *argv[]) {
    if (argc == 0) return;

    for (int i = 0; i < NUM_COMANDOS; i++) {
        if (strcmp(argv[0], mapa_de_comandos[i].nome) == 0) {
            mapa_de_comandos[i].funcao(argc, argv);
            return;
        }
    }
    printf("Comando desconhecido: '%s'.\n", argv[0]);
}