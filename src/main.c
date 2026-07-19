/* 
   Se chamado sem argumentos → abre a TUI interativa
   Se chamado com argumentos → executa o CLI diretamente*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>  /* API do Windows para console e cores ANSI */
#include <io.h>
#define isatty _isatty  /* equivalente do isatty no Windows */
#define fileno _fileno  /* equivalente do fileno no Windows */
#else
#include <unistd.h>     /* isatty e fileno no Linux/macOS */
#endif
#include "bible_data.h" /* struct Verso e array biblia[] */
#include "tui.h"        /* interface interativa */

/* códigos de escape ANSI para colorização no terminal */
#define NEGRITO  "\033[1m"
#define AMARELO  "\033[33m"
#define RESET    "\033[0m"

/* estrutura para mapear abreviações de livros para seus nomes completos
   ex: "gn" → "Gênesis", "joao" → "João" */
typedef struct {
    const char *alias;  /* abreviação digitada pelo usuário */
    const char *nome;   /* nome completo do livro no array biblia[] */
} Alias;

/* tabela completa de aliases — todos em minúsculo
   o usuário pode digitar qualquer variação e o programa encontra o livro */
static const Alias aliases[] = {
    {"gn",      "Gênesis"},   {"gen",      "Gênesis"},  {"genesis",  "Gênesis"},
    {"ex",      "Êxodo"},     {"exodo",    "Êxodo"},    {"exodus",   "Êxodo"},
    {"lv",      "Levítico"},  {"lev",      "Levítico"}, {"levitico", "Levítico"},
    {"nm",      "Números"},   {"num",      "Números"},  {"numeros",  "Números"},
    {"dt",      "Deuteronômio"}, {"deut",  "Deuteronômio"}, {"deuteronomio", "Deuteronômio"},
    {"js",      "Josué"},     {"josue",    "Josué"},
    {"jz",      "Juízes"},    {"juizes",   "Juízes"},
    {"rt",      "Rute"},      {"rute",     "Rute"},
    {"1sm",     "I Samuel"},  {"1sam",     "I Samuel"}, {"i samuel", "I Samuel"},
    {"2sm",     "II Samuel"}, {"2sam",     "II Samuel"},{"ii samuel","II Samuel"},
    {"1rs",     "I Reis"},    {"1reis",    "I Reis"},   {"i reis",   "I Reis"},
    {"2rs",     "II Reis"},   {"2reis",    "II Reis"},  {"ii reis",  "II Reis"},
    {"1cr",     "I Crônicas"},{"1cron",    "I Crônicas"},{"i cronicas","I Crônicas"},
    {"2cr",     "II Crônicas"},{"2cron",   "II Crônicas"},{"ii cronicas","II Crônicas"},
    {"ed",      "Esdras"},    {"esd",      "Esdras"},   {"esdras",   "Esdras"},
    {"ne",      "Neemias"},   {"nee",      "Neemias"},  {"neemias",  "Neemias"},
    {"tb",      "Tobias"},    {"tobias",   "Tobias"},
    {"jt",      "Judite"},    {"judite",   "Judite"},
    {"est",     "Ester"},     {"ester",    "Ester"},
    {"1mc",     "I Macabeus"},{"i macabeus","I Macabeus"},
    {"2mc",     "II Macabeus"},{"ii macabeus","II Macabeus"},
    {"jo",      "Jó"},        {"job",      "Jó"},
    {"sl",      "Salmos"},    {"sal",      "Salmos"},   {"salmo",    "Salmos"}, {"salmos","Salmos"},
    {"pv",      "Provérbios"},{"prov",     "Provérbios"},{"proverbios","Provérbios"},
    {"ecl",     "Eclesiastes"},{"eclesiastes","Eclesiastes"},
    {"ecli",    "Eclesiástico"},{"eclesiastico","Eclesiástico"},
    {"is",      "Isaías"},    {"isaias",   "Isaías"},
    {"jr",      "Jeremias"},  {"jeremias", "Jeremias"},
    {"lm",      "Lamentações"},{"lamentacoes","Lamentações"},
    {"bar",     "Baruc"},     {"baruc",    "Baruc"},
    {"ez",      "Ezequiel"},  {"ezequiel", "Ezequiel"},
    {"dn",      "Daniel"},    {"daniel",   "Daniel"},
    {"os",      "Oséias"},    {"oseias",   "Oséias"},
    {"jl",      "Joel"},      {"joel",     "Joel"},
    {"am",      "Amós"},      {"amos",     "Amós"},
    {"abd",     "Abdias"},    {"abdias",   "Abdias"},
    {"jn",      "Jonas"},     {"jonas",    "Jonas"},
    {"mq",      "Miquéias"},  {"miqueias", "Miquéias"},
    {"na",      "Naum"},      {"naum",     "Naum"},
    {"hab",     "Habacuc"},   {"habacuc",  "Habacuc"},
    {"sf",      "Sofonias"},  {"sofonias", "Sofonias"},
    {"ag",      "Ageu"},      {"ageu",     "Ageu"},
    {"zc",      "Zacarias"},  {"zacarias", "Zacarias"},
    {"ml",      "Malaquias"}, {"malaquias","Malaquias"},
    {"mt",      "Mateus"},    {"mateus",   "Mateus"},
    {"mc",      "Marcos"},    {"marcos",   "Marcos"},
    {"lc",      "Lucas"},     {"lucas",    "Lucas"},
    {"joao",    "João"},      {"joa",      "João"},     {"joão",     "João"},
    {"at",      "Atos"},      {"atos",     "Atos"},
    {"rm",      "Romanos"},   {"romanos",  "Romanos"},
    {"1cor",    "I Coríntios"},  {"1corintios","I Coríntios"},  {"i corintios","I Coríntios"},
    {"2cor",    "II Coríntios"}, {"2corintios","II Coríntios"}, {"ii corintios","II Coríntios"},
    {"gl",      "Gálatas"},   {"galatas",  "Gálatas"},
    {"ef",      "Efésios"},   {"efesios",  "Efésios"},
    {"fl",      "Filipenses"},{"filipenses","Filipenses"},
    {"cl",      "Colossenses"},{"colossenses","Colossenses"},
    {"1ts",     "I Tessalonicenses"},  {"1tess","I Tessalonicenses"},
    {"2ts",     "II Tessalonicenses"}, {"2tess","II Tessalonicenses"},
    {"1tm",     "I Timóteo"}, {"1tim",    "I Timóteo"},  {"i timoteo","I Timóteo"},
    {"2tm",     "II Timóteo"},{"2tim",    "II Timóteo"}, {"ii timoteo","II Timóteo"},
    {"tt",      "Tito"},      {"tito",    "Tito"},
    {"fm",      "Filêmon"},   {"filemon", "Filêmon"},
    {"hb",      "Hebreus"},   {"heb",     "Hebreus"},   {"hebreus",  "Hebreus"},
    {"tg",      "Tiago"},     {"tiago",   "Tiago"},
    {"1pe",     "I Pedro"},   {"1pedro",  "I Pedro"},   {"i pedro",  "I Pedro"},
    {"2pe",     "II Pedro"},  {"2pedro",  "II Pedro"},  {"ii pedro", "II Pedro"},
    {"1jo",     "I João"},    {"1joao",   "I João"},    {"i joao",   "I João"},
    {"2jo",     "II João"},   {"2joao",   "II João"},   {"ii joao",  "II João"},
    {"3jo",     "III João"},  {"3joao",   "III João"},  {"iii joao", "III João"},
    {"ap",      "Apocalipse"},{"apocalipse","Apocalipse"},
    {NULL, NULL}  
};

/* converte uma string para minúsculo in-place
   necessário para a busca ser case-insensitive (joao = Joao = JOAO) */
void lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 32;  /* diferença ASCII entre maiúscula e minúscula */
    }
}

/* percorre a tabela de aliases e retorna o nome completo do livro
   retorna NULL se o alias não for encontrado */
const char *resolver_livro(const char *entrada) {
    for (int i = 0; aliases[i].alias != NULL; i++) {
        if (strcmp(entrada, aliases[i].alias) == 0)
            return aliases[i].nome;
    }
    return NULL;
}

/* abre o paginador less se o output for um terminal
   se estiver em pipe (biblia sl 23 > arquivo.txt), usa stdout direto
   o -R permite que o less renderize as cores ANSI corretamente */
FILE *abrir_saida() {
    if (isatty(fileno(stdout))) {
        FILE *f = popen("less -R", "w");
        if (f) return f;
    }
    return stdout;
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    /* configura o terminal do Windows automaticamente — sem precisar rodar chcp manualmente */
    SetConsoleOutputCP(65001);  /* UTF-8 para output */
    SetConsoleCP(65001);        /* UTF-8 para input */

    /* habilita suporte a cores ANSI no terminal do Windows 10+ */
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    /* sem argumentos → abre a TUI interativa */
    if (argc < 2) {
        tui_run();
        return 0;
    }

    /* "biblia random"  imprime um verso aleatório e sai */
    if (strcmp(argv[1], "random") == 0) {
        srand((unsigned int)time(NULL));  /* seed com o tempo atual para aleatoriedade */
        int i = rand() % TOTAL;
        const Verso *v = &biblia[i];
        printf(AMARELO NEGRITO "%s %d:%d" RESET "\n%s\n\n",
            v->livro, v->capitulo, v->versiculo, v->texto);
        return 0;
    }

    /* "biblia /palavra" → busca o termo em todos os versículos
       o '/' no início do argumento indica busca por texto */
    if (argv[1][0] == '/') {
        const char *busca = argv[1] + 1;  /* pula o '/' */
        FILE *out = abrir_saida();
        int encontrou = 0;
        for (int i = 0; i < TOTAL; i++) {
            const Verso *v = &biblia[i];
            if (strstr(v->texto, busca)) {  /* strstr retorna NULL se não encontrar */
                fprintf(out, AMARELO NEGRITO "%s %d:%d" RESET "\n%s\n\n",
                    v->livro, v->capitulo, v->versiculo, v->texto);
                encontrou++;
            }
        }
        if (!encontrou)
            fprintf(out, "Nenhum resultado para: %s\n", busca);
        else
            fprintf(out, AMARELO "--- %d resultado(s) ---" RESET "\n", encontrou);
        if (out != stdout) pclose(out);
        return encontrou ? 0 : 1;
    }

    /* "biblia livro cap:vers" → busca por referência
       exemplos: "biblia joao 3:16", "biblia sl 23", "biblia gn 1" */
    char livro[64] = "";
    int cap = 0, vers = 0;

    /* o último argumento pode ser "cap:vers", "cap" ou parte do nome do livro */
    char *ref = argv[argc - 1];
    if (sscanf(ref, "%d:%d", &cap, &vers) == 2) {
        /* formato "livro cap:vers" — junta todos os args antes do último como nome do livro */
        for (int i = 1; i < argc - 1; i++) {
            if (i > 1) strcat(livro, " ");
            strcat(livro, argv[i]);
        }
    } else if (sscanf(ref, "%d", &cap) == 1 && argc > 2) {
        /* formato "livro cap" — junta todos os args antes do último */
        for (int i = 1; i < argc - 1; i++) {
            if (i > 1) strcat(livro, " ");
            strcat(livro, argv[i]);
        }
    } else {
        /* formato "livro" apenas — sem capítulo ou versículo */
        cap = 0;
        for (int i = 1; i < argc; i++) {
            if (i > 1) strcat(livro, " ");
            strcat(livro, argv[i]);
        }
    }

    /* converte o nome do livro para minúsculo antes de buscar na tabela */
    lowercase(livro);

    /* resolve o alias para o nome completo do livro */
    const char *livro_final = resolver_livro(livro);
    if (livro_final == NULL) {
        printf("Livro nao encontrado: %s\n", livro);
        return 1;
    }

    /* percorre o array biblia[] filtrando por livro, capítulo e versículo
       cap == 0 e vers == 0 significam "qualquer valor" (sem filtro) */
    FILE *out = abrir_saida();
    int encontrou = 0;
    for (int i = 0; i < TOTAL; i++) {
        const Verso *v = &biblia[i];
        int livro_ok = (strcmp(v->livro, livro_final) == 0);
        int cap_ok   = (cap == 0  || v->capitulo  == cap);
        int vers_ok  = (vers == 0 || v->versiculo == vers);
        if (livro_ok && cap_ok && vers_ok) {
            fprintf(out, AMARELO NEGRITO "%s %d:%d" RESET "\n%s\n\n",
                v->livro, v->capitulo, v->versiculo, v->texto);
            encontrou = 1;
        }
    }

    if (!encontrou)
        fprintf(out, "Nenhum resultado encontrado.\n");

    if (out != stdout) pclose(out);
    return encontrou ? 0 : 1;
}