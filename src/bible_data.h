#ifndef BIBLE_DATA_H
#define BIBLE_DATA_H

typedef struct {
    int livro_num;
    int capitulo;
    int versiculo;
    const char *livro;
    const char *texto;
} Verso;

extern const Verso biblia[];
extern const int TOTAL;

#endif
