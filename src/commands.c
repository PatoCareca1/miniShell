#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "commands.h"
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

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

// --- Helpers Internos ---

void imprimir_permissões(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

void imprimir_detalhes(const char *dir, const char *arquivo) {
    char caminho_completo[1024];
    struct stat st;
    struct passwd *pwd;
    struct group *grp;
    char tempo[26];

    // Monta o caminho completo para o stat funcionar
    snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", dir, arquivo);

    if (stat(caminho_completo, &st) == -1) return;

    pwd = getpwuid(st.st_uid);
    grp = getgrgid(st.st_gid);
    
    // Formata data e remove o \n final do ctime
    ctime_r(&st.st_mtime, tempo);
    tempo[strlen(tempo) - 1] = '\0';

    imprimir_permissões(st.st_mode);
    printf(" %ld", st.st_nlink);
    printf(" %s", (pwd) ? pwd->pw_name : "user");
    printf(" %s", (grp) ? grp->gr_name : "group");
    printf(" %5ld", st.st_size);
    printf(" %s", tempo);
    printf(" %s\n", arquivo);
}

// --- Comando ls ---

void cmd_ls(int argc, char *argv[]) {
    DIR *d;
    struct dirent *entry;
    const char *diretorio = ".";
    int mostrar_ocultos = 0; // -a
    int mostrar_detalhes = 0; // -l

    // Parse simples de argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) mostrar_ocultos = 1;
        else if (strcmp(argv[i], "-l") == 0) mostrar_detalhes = 1;
        else if (argv[i][0] != '-') diretorio = argv[i];
    }

    d = opendir(diretorio);
    if (!d) {
        perror("mini-shell: ls");
        return;
    }

    while ((entry = readdir(d)) != NULL) {
        // Pula ocultos se não tiver flag -a
        if (!mostrar_ocultos && entry->d_name[0] == '.') continue;

        if (mostrar_detalhes) {
            imprimir_detalhes(diretorio, entry->d_name);
        } else {
            printf("%s  ", entry->d_name);
        }
    }
    
    if (!mostrar_detalhes) printf("\n");
    closedir(d);
}

// --- Mapa de Comandos ---

static const Comando mapa_de_comandos[] = {
    { "exit",  cmd_exit,  "Sai do mini-shell" },
    { "pwd",   cmd_pwd,   "Mostra o diretorio atual" },
    { "mkdir", cmd_mkdir, "Cria um diretorio" },
    { "cd",    cmd_cd,    "Muda de diretorio" },
    { "ls",    cmd_ls,    "Lista arquivos (use -a ou -l)" },
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