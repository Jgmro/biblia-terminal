#ifndef BIBLE_DATA_H   /* evita incluir este arquivo mais de uma vez */
#define BIBLE_DATA_H

/* estrutura que representa um único versículo da Bíblia
   cada versículo no array biblia[] é uma instância desta struct */
typedef struct {
    int livro_num;       /* número do livro (1 = Gênesis, 2 = Êxodo, ..., 73 = Apocalipse) */
    int capitulo;        /* número do capítulo dentro do livro */
    int versiculo;       /* número do versículo dentro do capítulo */
    const char *livro;   /* nome do livro em português (ex: "João", "Salmos") */
    const char *texto;   /* texto completo do versículo */
} Verso;

/* array com todos os versículos da Bíblia — definido em bible_data.c
   gerado automaticamente a partir do TSV pelo script gerar.py
   compilado diretamente no binário — nenhum arquivo externo necessário */
extern const Verso biblia[];

/* número total de versículos no array biblia[]
   usado para limitar loops e gerar versos aleatórios */
extern const int TOTAL;

#endif 