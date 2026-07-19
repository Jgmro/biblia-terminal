#ifndef TUI_H      /* evita incluir este arquivo mais de uma vez */
#define TUI_H

#include <time.h>  /* necessário para usar time_t no verso aleatório */

/* códigos especiais para as teclas de seta do teclado
   usei valores altos (1000+) para não conflitar com caracteres ASCII normais */
#define KEY_UP   1000   /* seta para cima   */
#define KEY_DOWN 1001   /* seta para baixo  */

/* abre a interface TUI interativa — chamada quando o usuário roda "biblia" sem argumentos */
void tui_run(void);

/* ativa ou desativa o modo raw do terminal
   enable=1: lê tecla por tecla sem esperar Enter (necessário para a TUI)
   enable=0: restaura o comportamento normal do terminal */
void tui_raw_mode(int enable);

/* lê uma tecla pressionada pelo usuário
   retorna o código ASCII da tecla, ou KEY_UP/KEY_DOWN para as setas */
int tui_getch(void);

/* obtém o tamanho atual do terminal em linhas e colunas
   usado para ajustar o layout da TUI ao tamanho da janela */
void tui_get_size(int *rows, int *cols);

#endif 