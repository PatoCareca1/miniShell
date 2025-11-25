# 🐚 Mini-Shell em C

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

Um interpretador de comandos (Shell) modular, escalável e leve, desenvolvido em linguagem C. Este projeto explora o funcionamento interno de sistemas operativos UNIX, interagindo diretamente com o Kernel através de **System Calls**.

> **Nota:** Este projeto foi desenvolvido no âmbito da disciplina de **Sistemas Operacionais**.

---

## 📋 Índice

- [Sobre o Projeto](#-sobre-o-projeto)
- [Arquitetura e Design](#-arquitetura-e-design)
- [Estrutura de Ficheiros](#-estrutura-de-ficheiros)
- [Funcionalidades e Syscalls](#-funcionalidades-e-syscalls)
- [Como Rodar](#-como-rodar)
- [Como Contribuir](#-como-contribuir)

---

## 🚀 Sobre o Projeto

O objetivo deste Mini-Shell é simular um ambiente de linha de comando onde o utilizador pode executar operações de sistema de ficheiros, navegação e manipulação de texto.

Ao contrário de *shells* que apenas chamam outros programas (usando `system()` ou `exec`), este projeto **reimplementa** a lógica dos comandos internamente usando a API padrão do POSIX e C.

### Destaques
* **Zero Dependências Externas:** Usa apenas bibliotecas padrão do C e chamadas de sistema Linux.
* **Modularidade:** Adicionar um novo comando não exige alteração na lógica principal (`main.c`).
* **Alta Performance:** Manipulação direta de descritores de ficheiros e memória.

---

## 🏗 Arquitetura e Design

O diferencial deste projeto é a sua arquitetura baseada em **Data-Driven Programming** (Programação Orientada a Dados).

### O Problema do "Código Esparguete"
Em implementações tradicionais para iniciantes, é comum ver cadeias gigantes de `if/else if` dentro da função `main` para decidir qual comando executar. Isso torna o código difícil de ler e impossível de escalar.

### A Nossa Solução: Tabela de Comandos
Nós desacoplamos a lógica. O "motor" do shell não conhece os comandos; ele apenas consulta um mapa.

1.  **O Mapa (`commands.c`):** Existe um array de estruturas que mapeia uma *string* (ex: "ls") a um *ponteiro de função* (ex: `&cmd_ls`).
2.  **O Parser:** O shell lê a linha, quebra em tokens (argumentos) e procura a chave no mapa.
3.  **Execução:** Se encontrada, a função correspondente é disparada automaticamente.

Isso permite adicionar novos comandos apenas registando-os na tabela, mantendo o `main.c` limpo e focado apenas no ciclo de vida da aplicação (Loop -> Read -> Eval -> Print).

---

## 📂 Estrutura de Ficheiros

O projeto segue uma organização profissional de diretórios para separar interfaces de implementações:

```plaintext
miniShell/
├── Makefile            # Automação de compilação (Build System)
├── include/            # (Headers) Contratos e definições de tipos
│   ├── commands.h      # Definição da struct Comando e protótipos
│   └── ui.h            # Interface de utilizador (ASCII art, etc)
├── src/                # (Source) Implementação lógica
│   ├── main.c          # Ponto de entrada e ciclo principal (REPL)
│   ├── commands.c      # Implementação de todos os comandos e do mapa
│   └── ui.c            # Implementação visual
└── obj/                # (Gerado) Ficheiros objeto compilados (.o)
````

-----

## 🛠 Funcionalidades e Syscalls

Abaixo, a lista de comandos implementados e as principais **Chamadas de Sistema (Syscalls)** ou funções de biblioteca utilizadas para operar diretamente no SO.

### 🔹 Navegação e Sistema

| Comando | Descrição | Syscalls / Libs Principais |
| :--- | :--- | :--- |
| `exit` | Encerra o shell | `exit()` |
| `pwd` | Exibe diretório atual | `getcwd()` |
| `cd` | Muda de diretório | `chdir()` |

### 🔹 Gestão de Diretórios

| Comando | Descrição | Syscalls / Libs Principais |
| :--- | :--- | :--- |
| `mkdir` | Cria um diretório | `mkdir()` (com permissão 0755) |
| `rmdir` | Remove diretório vazio | `rmdir()` |
| `ls` | Lista arquivos (`-a`, `-l`) | `opendir()`, `readdir()`, `stat()`, `closedir()` |

### 🔹 Manipulação de Ficheiros

| Comando | Descrição | Syscalls / Libs Principais |
| :--- | :--- | :--- |
| `cp` | Copia ficheiros | `open()`, `read()`, `write()`, `close()` |
| `mv` | Move ou Renomeia | `rename()` |
| `rm` | Remove ficheiro | `unlink()` |

### 🔹 Processamento de Texto

| Comando | Descrição | Syscalls / Libs Principais |
| :--- | :--- | :--- |
| `cat` | Exibe conteúdo | `open()`, `read()`, `write(STDOUT)` |
| `grep` | Busca texto em ficheiro | `fopen()`, `fgets()`, `strstr()` |
| `sort` | Ordena linhas (RAM) | `fopen()`, `fgets()`, `qsort()` |

-----

## 💻 Como Rodar

### Pré-requisitos

  * Sistema Operativo: **Linux** (ou WSL no Windows, macOS).
  * Compilador: **GCC**.
  * Ferramenta: **Make**.

### Passo a Passo

1.  **Clone o repositório** (ou baixe os ficheiros):

    ```bash
    git clone [https://github.com/teu-usuario/mini-shell.git](https://github.com/teu-usuario/mini-shell.git)
    cd mini-shell
    ```

2.  **Compile o projeto**:
    Utilizamos um `Makefile` inteligente que compila apenas o necessário.

    ```bash
    make
    ```

    *Se quiser limpar a compilação anterior, use `make clean`.*

3.  **Execute o Shell**:

    ```bash
    ./miniShell
    ```

4.  **Exemplo de Uso**:

    ```bash
    mini-shell> mkdir teste
    mini-shell> cd teste
    mini-shell> ls -l
    mini-shell> exit
    ```

-----

## 🤝 Como Contribuir

Este projeto foi desenhado para ser **escalável**. Queres adicionar um comando novo (ex: `date` ou `echo`)?

1.  Abra `src/commands.c`.
2.  Crie a função `void cmd_novo(int argc, char *argv[]) { ... }`.
3.  Adicione a linha no array `mapa_de_comandos`:
    ```c
    { "novo", cmd_novo, "Descrição do comando" },
    ```
4.  Recompile com `make`.

Pronto\! O sistema reconhece o novo comando automaticamente sem mexer em mais nada.

-----

**Desenvolvido por Lucas Daniel Costa Souza**
*Trabalho Prático - Sistemas Operacionais - 2025*
