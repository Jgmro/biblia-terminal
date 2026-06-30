#ifndef TUI_H
#define TUI_H

#include <time.h>

#define KEY_UP   1000
#define KEY_DOWN 1001

void tui_run(void);
void tui_raw_mode(int enable);
int  tui_getch(void);
void tui_get_size(int *rows, int *cols);

#endif