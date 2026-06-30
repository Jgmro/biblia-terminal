#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif
#include "bible_data.h"
#include "tui.h"

#define NEGRITO  "\033[1m"
#define AMARELO  "\033[33m"
#define RESET    "\033[0m"

typedef struct {
    const char *alias;
    const char *nome;
} Alias;

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

void lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 32;
    }
}

const char *resolver_livro(const char *entrada) {
    for (int i = 0; aliases[i].alias != NULL; i++) {
        if (strcmp(entrada, aliases[i].alias) == 0)
            return aliases[i].nome;
    }
    return NULL;
}

FILE *abrir_saida() {
    if (isatty(fileno(stdout))) {
        FILE *f = popen("less -R", "w");
        if (f) return f;
    }
    return stdout;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        tui_run();
        return 0;
    }

    /* Verso aleatorio */
    if (strcmp(argv[1], "random") == 0) {
        srand((unsigned int)time(NULL));
        int i = rand() % TOTAL;
        const Verso *v = &biblia[i];
        printf(AMARELO NEGRITO "%s %d:%d" RESET "\n%s\n\n",
            v->livro, v->capitulo, v->versiculo, v->texto);
        return 0;
    }

    /* Busca por texto */
    if (argv[1][0] == '/') {
        const char *busca = argv[1] + 1;
        FILE *out = abrir_saida();
        int encontrou = 0;
        for (int i = 0; i < TOTAL; i++) {
            const Verso *v = &biblia[i];
            if (strstr(v->texto, busca)) {
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

    /* Busca por referencia */
    char livro[64] = "";
    int cap = 0, vers = 0;

    char *ref = argv[argc - 1];
    if (sscanf(ref, "%d:%d", &cap, &vers) == 2) {
        for (int i = 1; i < argc - 1; i++) {
            if (i > 1) strcat(livro, " ");
            strcat(livro, argv[i]);
        }
    } else if (sscanf(ref, "%d", &cap) == 1 && argc > 2) {
        for (int i = 1; i < argc - 1; i++) {
            if (i > 1) strcat(livro, " ");
            strcat(livro, argv[i]);
        }
    } else {
        cap = 0;
        for (int i = 1; i < argc; i++) {
            if (i > 1) strcat(livro, " ");
            strcat(livro, argv[i]);
        }
    }

    lowercase(livro);

    const char *livro_final = resolver_livro(livro);
    if (livro_final == NULL) {
        printf("Livro nao encontrado: %s\n", livro);
        return 1;
    }

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