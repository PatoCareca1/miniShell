# 🐚 Mini-Shell em C

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

Um interpretador de comandos (Shell) modular, escalável e seguro, desenvolvido em linguagem C. Este projeto simula um ambiente UNIX completo, interagindo diretamente com o Kernel através de **System Calls** e implementando um sistema de **Sandbox** para execução segura.

> **Nota:** Desenvolvido para a disciplina de **Sistemas Operacionais**.

---

## 📋 Índice

- [Destaques Técnicos](#-destaques-técnicos)
- [Arquitetura e Sandbox](#-arquitetura-e-sandbox)
- [Estrutura do Projeto](#-estrutura-do-projeto)
- [Funcionalidades e Syscalls](#-funcionalidades-e-syscalls)
- [Guia de Instalação e Uso](#-guia-de-instalação-e-uso)
- [Exemplos de Comandos](#-exemplos-de-comandos)

---

## 🚀 Destaques Técnicos

Este projeto vai além de um shell básico, implementando conceitos avançados de sistemas:

* **🛡️ Ambiente Sandbox Automático:** Ao iniciar, o shell cria e isola a execução dentro de um diretório `sandbox/`. Todas as operações de ficheiros (criar, apagar, mover) ficam contidas, garantindo segurança e limpeza fácil.
* **🎨 Feedback Visual (UI):** Interface colorida usando códigos ANSI para feedback imediato (Verde para sucesso, Vermelho para erros).
* **🔀 Redirecionamento de I/O:** O comando `echo` implementa um parser próprio que suporta redirecionamento de saída (`>`) para criar ficheiros.
* **📝 Interpretação de Escape:** Suporte a caracteres especiais como quebra de linha (`\n`) dentro de strings.
* **🏗️ Arquitetura Orientada a Dados:** O "motor" do shell é desacoplado da lógica dos comandos, permitindo escalabilidade infinita através de uma tabela de ponteiros de função.

---

## 🏗 Arquitetura e Sandbox

### O Conceito de Sandbox
Para evitar poluir o diretório do projeto ou o sistema do utilizador, o `main.c` executa a seguinte rotina de inicialização:
1.  Verifica a existência do diretório `sandbox/`.
2.  Cria-o via `mkdir()` se não existir.
3.  Muda o diretório de trabalho do processo (`chdir()`) para dentro dele.

### O Ciclo de Vida (REPL)
1.  **Read:** Lê o input do utilizador (`fgets`).
2.  **Parse:** Quebra a string em tokens e interpreta caracteres de escape.
3.  **Eval:** Consulta a Tabela de Comandos para encontrar a função correspondente.
4.  **Print:** Executa a função (usando `syscalls`) e exibe o resultado colorido.

---

## 📂 Estrutura do Projeto

```plaintext
miniShell/
├── Makefile            # Automação de build e gestão de ambiente
├── include/            # Contratos (Headers)
│   ├── commands.h      # Definição da struct Comando e protótipos
│   └── ui.h            # Definições de cores ANSI e interface
├── src/                # Implementação
│   ├── main.c          # Inicialização do Sandbox e Loop Principal
│   ├── commands.c      # Implementação lógica de todos os comandos
│   └── ui.c            # Funções visuais
└── sandbox/            # (Gerado Automaticamente) Onde os arquivos são criados
````

-----

## 🛠 Funcionalidades e Syscalls

Abaixo, a lista de comandos implementados e as chamadas de sistema (**System Calls**) utilizadas.

### 🔹 Sistema e Ajuda

| Comando | Descrição | Implementação / Syscalls |
| :--- | :--- | :--- |
| `help` | Lista todos os comandos e flags | Tabela de Structs + Cores ANSI |
| `exit` | Encerra o shell | `exit()` |
| `pwd` | Exibe diretório atual | `getcwd()` |

### 🔹 Gestão de Diretórios e Ficheiros

| Comando | Descrição | Implementação / Syscalls |
| :--- | :--- | :--- |
| `mkdir` | Cria um diretório | `mkdir()` |
| `rmdir` | Remove diretório vazio | `rmdir()` |
| `cd` | Navega entre pastas | `chdir()` |
| `ls` | Lista conteúdo (`-a`, `-l`) | `opendir()`, `readdir()`, `stat()`, `closedir()` |
| `touch` | Cria arquivo vazio | `open(O_CREAT)`, `close()` |
| `rm` | Remove ficheiro | `unlink()` |
| `cp` | Copia ficheiro (byte-a-byte) | `open()`, `read()`, `write()`, `close()` |
| `mv` | Move ou Renomeia | `rename()` |

### 🔹 Processamento de Texto Avançado

| Comando | Descrição | Detalhes Técnicos |
| :--- | :--- | :--- |
| `cat` | Exibe conteúdo | `read()`, `write(STDOUT)` |
| `grep` | Busca termo em arquivo | `fopen()`, `fgets()`, `strstr()` |
| `sort` | Ordena linhas (RAM) | `qsort()`, `strcmp()` |
| `echo` | Imprime texto | Suporta flag `-n`, escape `\n` e operador `>` |

-----

## 💻 Guia de Instalação e Uso

### Pré-requisitos

  * **Linux** (ou WSL/macOS).
  * **GCC** e **Make**.

### Como Rodar

1.  **Clone e compile:**

    ```
    git clone https://github.com/PatoCareca1/miniShell.git
    cd mini-shell

2.  **Inicie o Ambiente (Build + Run):**
    O comando abaixo compila o projeto, cria a pasta `sandbox` e inicia o shell dentro dela.

    ```
    make run

3.  **Limpeza Total:**
    Para remover os binários compilados E apagar todo o conteúdo criado na sandbox:

    ```
    make clean

-----

## 🧪 Exemplos de Comandos

Experimente estas sequências para testar as capacidades do shell:

**1. Criar ficheiros com conteúdo (Redirecionamento):**

```
mini-shell> echo Lista de Compras: > lista.txt
mini-shell> echo -n Pao > item1.txt
```

**2. Usar caracteres de escape (Quebra de linha):**

```
mini-shell> echo Arroz\nFeijao\nMacarrao > comida.txt
mini-shell> cat comida.txt
```

**3. Manipulação e Listagem:**

```
mini-shell> mkdir backup
mini-shell> cp comida.txt backup/comida_bkp.txt
mini-shell> ls -l
```

-----

**Desenvolvido por Lucas Daniel**
