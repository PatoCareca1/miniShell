#ifndef UI_H
#define UI_H

// Códigos ANSI para cores no terminal
#define COR_RESET    "\033[0m"
#define COR_VERDE    "\033[1;32m"
#define COR_AZUL     "\033[1;34m"
#define COR_AMARELO  "\033[1;33m"
#define COR_VERMELHO "\033[1;31m"
#define COR_CYAN     "\033[1;36m"
#define COR_BOLD     "\033[1m"

// Função de boas-vindas
void ui_mostrar_boas_vindas(void);

#endif // UI_Hei