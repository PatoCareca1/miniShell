#include <stdio.h>
#include "ui.h"

void ui_mostrar_boas_vindas(void) {
    printf(COR_CYAN "========================================\n");
    printf("==                                    ==\n");
    printf("==   " COR_AMARELO "MINI-SHELL - SISTEMAS OP." COR_CYAN "      ==\n");
    printf("==                                    ==\n");
    printf("========================================" COR_RESET "\n");
    printf("Digite " COR_VERDE "help" COR_RESET " para ver os comandos.\n\n");
}