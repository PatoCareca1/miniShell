#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // getcwd, chdir, unlink, rmdir, read, write, close
#include <sys/stat.h>   // mkdir, stat
#include <errno.h>
#include <dirent.h>     // opendir, readdir, closedir
#include <time.h>       // ctime
#include <pwd.h>        // getpwuid
#include <grp.h>        // getgrgid
#include <fcntl.h>      // open, O_RDONLY, etc
#include "commands.h"

// --- COMANDOS DE SISTEMA E NAVEGAÇÃO ---

// Syscall: exit (library)
void cmd_exit(int argc, char *argv[]) {
    printf("Ate logo!\n");
    exit(0);
}

// Syscall: getcwd
void cmd_pwd(int argc, char *argv[]) {
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        printf("%s\n", buffer);
    } else {
        perror("mini-shell: pwd");
    }
}

// Syscall: mkdir
void cmd_mkdir(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: mkdir <nome_do_diretorio>\n");
        return;
    }
    // Cria diretório com permissão 0755
    if (mkdir(argv[1], 0755) == 0) {
        printf("Diretorio '%s' criado com sucesso.\n", argv[1]);
    } else {
        perror("mini-shell: mkdir");
    }
}

// Syscall: chdir
void cmd_cd(int argc, char *argv[]) {
    char *caminho;
    
    if (argc < 2) {
        caminho = getenv("HOME");
        if (!caminho) {
            printf("mini-shell: cd: HOME nao definida\n");
            return;
        }
    } else {
        caminho = argv[1];
    }

    if (chdir(caminho) != 0) {
        perror("mini-shell: cd");
    }
}

// --- COMANDO LS (HELPERS E FUNÇÃO) ---

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

// Syscalls: stat, getpwuid, getgrgid, ctime
void imprimir_detalhes(const char *dir, const char *arquivo) {
    char caminho_completo[1024];
    struct stat st;
    struct passwd *pwd;
    struct group *grp;
    char tempo[26];

    snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", dir, arquivo);

    if (stat(caminho_completo, &st) == -1) return;

    pwd = getpwuid(st.st_uid);
    grp = getgrgid(st.st_gid);
    ctime_r(&st.st_mtime, tempo);
    tempo[strlen(tempo) - 1] = '\0'; // Remove \n

    imprimir_permissões(st.st_mode);
    printf(" %ld %s %s %5ld %s %s\n", 
           st.st_nlink, 
           (pwd) ? pwd->pw_name : "user", 
           (grp) ? grp->gr_name : "group", 
           st.st_size, tempo, arquivo);
}

// Syscalls: opendir, readdir, closedir
void cmd_ls(int argc, char *argv[]) {
    DIR *d;
    struct dirent *entry;
    const char *diretorio = ".";
    int mostrar_ocultos = 0;
    int mostrar_detalhes = 0;

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

// --- MANIPULAÇÃO DE ARQUIVOS E DIRETÓRIOS ---

// Syscall: rmdir
void cmd_rmdir(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: rmdir <diretorio>\n");
        return;
    }
    if (rmdir(argv[1]) != 0) {
        perror("mini-shell: rmdir");
    }
}

// Syscall: unlink
void cmd_rm(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: rm <arquivo>\n");
        return;
    }
    if (unlink(argv[1]) != 0) {
        perror("mini-shell: rm");
    }
}

// Syscall: rename
void cmd_mv(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: mv <origem> <destino>\n");
        return;
    }
    if (rename(argv[1], argv[2]) != 0) {
        perror("mini-shell: mv");
    }
}

// Syscalls: open, read, write, close
void cmd_cp(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: cp <origem> <destino>\n");
        return;
    }

    int fd_origem = open(argv[1], O_RDONLY);
    if (fd_origem == -1) {
        perror("mini-shell: cp (origem)");
        return;
    }

    int fd_destino = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd_destino == -1) {
        perror("mini-shell: cp (destino)");
        close(fd_origem);
        return;
    }

    char buffer[4096];
    ssize_t bytes_lidos;
    while ((bytes_lidos = read(fd_origem, buffer, sizeof(buffer))) > 0) {
        if (write(fd_destino, buffer, bytes_lidos) != bytes_lidos) {
            perror("mini-shell: cp (escrita)");
            break;
        }
    }

    close(fd_origem);
    close(fd_destino);
}

// --- PROCESSAMENTO DE TEXTO ---

// Syscalls: open, read, write, close
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
        write(STDOUT_FILENO, buffer, lidos);
    }
    close(fd);
}

// Lib Functions: fopen, fgets, strstr, fclose
void cmd_grep(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: grep <termo> <arquivo>\n");
        return;
    }

    FILE *fp = fopen(argv[2], "r");
    if (!fp) {
        perror("mini-shell: grep");
        return;
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), fp)) {
        if (strstr(linha, argv[1]) != NULL) {
            printf("%s", linha);
        }
    }
    fclose(fp);
}

int comparar_linhas(const void *a, const void *b) {
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}

// Lib Functions: fopen, fgets, qsort, fclose
void cmd_sort(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: sort <arquivo>\n");
        return;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("mini-shell: sort");
        return;
    }

    char *linhas[1000]; 
    int count = 0;
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), fp) && count < 1000) {
        linhas[count] = strdup(buffer);
        if (linhas[count]) count++;
    }
    fclose(fp);

    qsort(linhas, count, sizeof(char *), comparar_linhas);

    for (int i = 0; i < count; i++) {
        printf("%s", linhas[i]);
        free(linhas[i]);
    }
}

// --- MAPA DE COMANDOS ---

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