#ifndef COMMANDS_H
#define COMMANDS_H

void comandos_executar(int argc, char *argv[]);

typedef struct {
    const char *nome;
    void (*funcao)(int argc, char *argv[]);
    const char *descricao;
} Comando;

// --- Funções de Comando ---

void cmd_exit(int argc, char *argv[]);
void cmd_pwd(int argc, char *argv[]);
void cmd_mkdir(int argc, char *argv[]);
void cmd_cd(int argc, char *argv[]);
void cmd_ls(int argc, char *argv[]);
void cmd_cp(int argc, char *argv[]);
void cmd_mv(int argc, char *argv[]);
void cmd_rm(int argc, char *argv[]);
void cmd_rmdir(int argc, char *argv[]);

#endif