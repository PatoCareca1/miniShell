#ifndef COMMANDS_H
#define COMMANDS_H

// Definição da estrutura de um comando
typedef struct {
    const char *nome;          // O nome que o utilizador digita
    void (*funcao)(void);      // Ponteiro para a função a executar
    const char *descricao;     // Descrição para o menu 'ajuda'
} Comando;


// Função principal que recebe o input do utilizador e decide o que fazer
void comandos_executar(const char *input);

// Função que será chamada pelo comando 'ajuda'
void cmd_ajuda(void);

// Função que será chamada pelo comando 'sair'
void cmd_sair(void);

// (PODES ADICIONAR MAIS FUNÇÕES DE COMANDO AQUI)
// Ex: void cmd_listar_utilizadores(void);

#endif // COMMANDS_H