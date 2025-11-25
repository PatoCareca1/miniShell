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
#include <fcntl.h>
#include <unistd.h>

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

// --- Comandos de Manipulação de Ficheiros ---

/**
 * @brief Remove um diretório vazio.
 * Syscall: rmdir
 */
void cmd_rmdir(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: rmdir <diretorio>\n");
        return;
    }
    if (rmdir(argv[1]) != 0) {
        perror("mini-shell: rmdir");
    }
}

/**
 * @brief Remove um arquivo.
 * Syscall: unlink
 */
void cmd_rm(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: rm <arquivo>\n");
        return;
    }
    // unlink é a syscall para remover o "link" do arquivo (apagá-lo)
    if (unlink(argv[1]) != 0) {
        perror("mini-shell: rm");
    }
}

/**
 * @brief Move ou renomeia arquivos/diretórios.
 * Syscall: rename
 */
void cmd_mv(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: mv <origem> <destino>\n");
        return;
    }
    // rename funciona tanto para mover como para renomear
    if (rename(argv[0+1], argv[2]) != 0) { // argv[1] é origem, argv[2] é destino
        perror("mini-shell: mv");
    }
}

/**
 * @brief Copia um arquivo (byte a byte).
 * Syscalls: open, read, write, close
 */
void cmd_cp(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: cp <origem> <destino>\n");
        return;
    }

    int fd_origem, fd_destino;
    char buffer[4096]; // Buffer de 4KB para transferência
    ssize_t bytes_lidos;

    // 1. Abrir origem para leitura
    fd_origem = open(argv[1], O_RDONLY);
    if (fd_origem == -1) {
        perror("mini-shell: cp (abrir origem)");
        return;
    }

    // 2. Abrir/Criar destino para escrita
    // O_CREAT: cria se não existir
    // O_WRONLY: apenas escrita
    // O_TRUNC: apaga o conteúdo anterior se já existir
    // 0644: permissões rw-r--r--
    fd_destino = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd_destino == -1) {
        perror("mini-shell: cp (criar destino)");
        close(fd_origem);
        return;
    }

    // 3. Loop de cópia
    while ((bytes_lidos = read(fd_origem, buffer, sizeof(buffer))) > 0) {
        if (write(fd_destino, buffer, bytes_lidos) != bytes_lidos) {
            perror("mini-shell: cp (erro de escrita)");
            break;
        }
    }

    // 4. Fechar descritores
    close(fd_origem);
    close(fd_destino);
}

// --- Comandos de Processamento de Texto ---

/**
 * @brief Exibe o conteúdo de um arquivo.
 * Usa syscalls read/write para eficiência e demonstração.
 */
void cmd_cat(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: cat <arquivo>\n");
        return;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("mini-shell: cat");
        return;
    }

    char buffer[4096];
    ssize_t lidos;

    while ((lidos = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, lidos); // Escreve direto na saída padrão
    }

    close(fd);
}

/**
 * @brief Busca uma string dentro de um arquivo.
 * Usa fgets para leitura linha a linha.
 */
void cmd_grep(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: grep <termo> <arquivo>\n"); // [cite: 21]
        return;
    }

    const char *termo = argv[1];
    const char *arquivo = argv[2];

    FILE *fp = fopen(arquivo, "r");
    if (!fp) {
        perror("mini-shell: grep");
        return;
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), fp)) {
        // strstr retorna ponteiro se encontrar a substring, NULL se não
        if (strstr(linha, termo) != NULL) {
            printf("%s", linha);
        }
    }

    fclose(fp);
}

// Helper para o qsort do comando sort
int comparar_linhas(const void *a, const void *b) {
    // Cast para ponteiros de strings
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}

/**
 * @brief Lê um arquivo, ordena as linhas e imprime.
 * Limitação: Máximo 1000 linhas de 1024 chars para simplicidade.
 */
void cmd_sort(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: sort <arquivo>\n"); // [cite: 21]
        return;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("mini-shell: sort");
        return;
    }

    // Array de ponteiros para as linhas
    char *linhas[1000]; 
    int count = 0;
    char buffer[1024];

    // 1. Carregar arquivo para memória
    while (fgets(buffer, sizeof(buffer), fp) && count < 1000) {
        linhas[count] = strdup(buffer); // Aloca memória para a linha
        if (linhas[count]) count++;
    }
    fclose(fp);

    // 2. Ordenar usando Quick Sort (função padrão do C)
    qsort(linhas, count, sizeof(char *), comparar_linhas);

    // 3. Imprimir e libertar memória
    for (int i = 0; i < count; i++) {
        printf("%s", linhas[i]);
        free(linhas[i]); // Importante: liberar o que o strdup alocou
    }
}

// --- Mapa de Comandos ---

static const Comando mapa_de_comandos[] = {
    // Sistema
    { "exit",  cmd_exit,  "Sai do mini-shell" },
    { "pwd",   cmd_pwd,   "Mostra o diretorio atual" },
    { "cd",    cmd_cd,    "Muda de diretorio" },
    
    // Gestão de Diretórios
    { "mkdir", cmd_mkdir, "Cria um diretorio" },
    { "rmdir", cmd_rmdir, "Remove diretorio vazio" },
    { "ls",    cmd_ls,    "Lista arquivos" },
    
    // Gestão de Arquivos
    { "cp",    cmd_cp,    "Copia arquivo" },
    { "mv",    cmd_mv,    "Move ou renomeia" },
    { "rm",    cmd_rm,    "Remove arquivo" },
    
    // Processamento de Texto
    { "cat",   cmd_cat,   "Mostra conteudo do arquivo" },
    { "grep",  cmd_grep,  "Busca texto em arquivo" },
    { "sort",  cmd_sort,  "Ordena conteudo do arquivo" },
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