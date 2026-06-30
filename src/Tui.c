#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include "bible_data.h"
#include "tui.h"

#define CLR      "\033[2J\033[H"
#define HIDE_CUR "\033[?25l"
#define SHOW_CUR "\033[?25h"
#define BOLD     "\033[1m"
#define DIM      "\033[2m"
#define YELLOW   "\033[33m"
#define CYAN     "\033[36m"
#define GREEN    "\033[32m"
#define RESET    "\033[0m"

typedef struct { const char *alias; const char *nome; } AliasLivro;
static const AliasLivro ALIASES[] = {
    {"gn","Gênesis"},{"gen","Gênesis"},{"genesis","Gênesis"},
    {"ex","Êxodo"},{"exodo","Êxodo"},
    {"lv","Levítico"},{"levitico","Levítico"},
    {"nm","Números"},{"numeros","Números"},
    {"dt","Deuteronômio"},{"deuteronomio","Deuteronômio"},
    {"js","Josué"},{"josue","Josué"},
    {"jz","Juízes"},{"juizes","Juízes"},
    {"rt","Rute"},{"rute","Rute"},
    {"1sm","I Samuel"},{"1sam","I Samuel"},
    {"2sm","II Samuel"},{"2sam","II Samuel"},
    {"1rs","I Reis"},{"1reis","I Reis"},
    {"2rs","II Reis"},{"2reis","II Reis"},
    {"1cr","I Crônicas"},{"2cr","II Crônicas"},
    {"ed","Esdras"},{"esdras","Esdras"},
    {"ne","Neemias"},{"neemias","Neemias"},
    {"tb","Tobias"},{"tobias","Tobias"},
    {"jt","Judite"},{"judite","Judite"},
    {"est","Ester"},{"ester","Ester"},
    {"1mc","I Macabeus"},{"2mc","II Macabeus"},
    {"jo","Jó"},{"job","Jó"},
    {"sl","Salmos"},{"sal","Salmos"},{"salmo","Salmos"},{"salmos","Salmos"},
    {"pv","Provérbios"},{"prov","Provérbios"},{"proverbios","Provérbios"},
    {"ecl","Eclesiastes"},{"eclesiastes","Eclesiastes"},
    {"ecli","Eclesiástico"},{"eclesiastico","Eclesiástico"},
    {"is","Isaías"},{"isaias","Isaías"},
    {"jr","Jeremias"},{"jeremias","Jeremias"},
    {"lm","Lamentações"},{"lamentacoes","Lamentações"},
    {"bar","Baruc"},{"baruc","Baruc"},
    {"ez","Ezequiel"},{"ezequiel","Ezequiel"},
    {"dn","Daniel"},{"daniel","Daniel"},
    {"os","Oséias"},{"oseias","Oséias"},
    {"jl","Joel"},{"joel","Joel"},
    {"am","Amós"},{"amos","Amós"},
    {"abd","Abdias"},{"abdias","Abdias"},
    {"jn","Jonas"},{"jonas","Jonas"},
    {"mq","Miquéias"},{"miqueias","Miquéias"},
    {"na","Naum"},{"naum","Naum"},
    {"hab","Habacuc"},{"habacuc","Habacuc"},
    {"sf","Sofonias"},{"sofonias","Sofonias"},
    {"ag","Ageu"},{"ageu","Ageu"},
    {"zc","Zacarias"},{"zacarias","Zacarias"},
    {"ml","Malaquias"},{"malaquias","Malaquias"},
    {"mt","Mateus"},{"mateus","Mateus"},
    {"mc","Marcos"},{"marcos","Marcos"},
    {"lc","Lucas"},{"lucas","Lucas"},
    {"joao","João"},{"joa","João"},
    {"at","Atos"},{"atos","Atos"},
    {"rm","Romanos"},{"romanos","Romanos"},
    {"1cor","I Coríntios"},{"1corintios","I Coríntios"},
    {"2cor","II Coríntios"},{"2corintios","II Coríntios"},
    {"gl","Gálatas"},{"galatas","Gálatas"},
    {"ef","Efésios"},{"efesios","Efésios"},
    {"fl","Filipenses"},{"filipenses","Filipenses"},
    {"cl","Colossenses"},{"colossenses","Colossenses"},
    {"1ts","I Tessalonicenses"},{"2ts","II Tessalonicenses"},
    {"1tm","I Timóteo"},{"1tim","I Timóteo"},
    {"2tm","II Timóteo"},{"2tim","II Timóteo"},
    {"tt","Tito"},{"tito","Tito"},
    {"fm","Filêmon"},{"filemon","Filêmon"},
    {"hb","Hebreus"},{"heb","Hebreus"},{"hebreus","Hebreus"},
    {"tg","Tiago"},{"tiago","Tiago"},
    {"1pe","I Pedro"},{"1pedro","I Pedro"},
    {"2pe","II Pedro"},{"2pedro","II Pedro"},
    {"1jo","I João"},{"1joao","I João"},
    {"2jo","II João"},{"2joao","II João"},
    {"3jo","III João"},{"3joao","III João"},
    {"ap","Apocalipse"},{"apocalipse","Apocalipse"},
    {NULL,NULL}
};

static const char *LIVRO_ART[] = {
    "          _______       ",
    "         /       /_    ",
    "        /  -/-  / /    ",
    "       /   /   / /     ",
    "      /_______/ /      ",
    "     ((______| /       ",
    "      `\"\"\"\"\"\"\"'        ",
    NULL
};

static const char *POMBA_ART[] = {
    " _      xxxx      _    ",
    "/_;-.__ / _\\  _.-;_\\  ",
    "   `-._`'`_/'`.-'      ",
    "       `\\   /`         ",
    "        |  /           ",
    "       /-(             ",
    "       \\_._\\           ",
    "        \\ \\`;          ",
    "         > |/          ",
    "        / //           ",
    "        |//            ",
    "        \\(\\            ",
    "         ``            ",
    NULL
};

static const char *HEADER[] = {
    "╔═══════════════════════════╗",
    "║    ✝  B Í B L I A  ✝     ║",
    "║      IC  ·  XC  ·  NIKA  ║",
    "╚═══════════════════════════╝",
    NULL
};

/* ── TERMINAL ─────────────────────────────────────────── */
#ifdef _WIN32
static DWORD orig_mode;

void tui_raw_mode(int enable) {
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    if (enable) {
        GetConsoleMode(h, &orig_mode);
        SetConsoleMode(h, orig_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    } else {
        SetConsoleMode(h, orig_mode);
    }
}

int tui_getch() {
    int c = _getch();
    if (c == 0 || c == 224) {
        int c2 = _getch();
        if (c2 == 72) return KEY_UP;
        if (c2 == 80) return KEY_DOWN;
    }
    return c;
}

void tui_get_size(int *rows, int *cols) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

#else
static struct termios orig_termios;

void tui_raw_mode(int enable) {
    if (enable) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }
}

int tui_getch() {
    int c = getchar();
    if (c == 27) {
        int c2 = getchar();
        if (c2 == '[') {
            int c3 = getchar();
            if (c3 == 'A') return KEY_UP;
            if (c3 == 'B') return KEY_DOWN;
        }
        return 27;
    }
    return c;
}

void tui_get_size(int *rows, int *cols) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *rows = w.ws_row;
    *cols = w.ws_col;
}
#endif

/* ── HELPERS ──────────────────────────────────────────── */
static void print_art(const char **art, const char *color) {
    for (int i = 0; art[i]; i++)
        printf("%s%s%s\n", color, art[i], RESET);
}

static void print_sep(int cols) {
    printf(YELLOW);
    int n = cols > 60 ? 60 : cols;
    for (int i = 0; i < n; i++) printf("─");
    printf(RESET "\n");
}

static void print_rodape() {
    printf(DIM " [↑↓/jk] Scroll  [/] Buscar  [r] Aleatório\n");
    printf(" [f] Favorito     [?] Ajuda   [q] Sair\n" RESET);
}

static void salvar_fav(const Verso *v) {
    char path[256];
#ifdef _WIN32
    snprintf(path, sizeof(path), "C:\\Users\\%s\\.biblia_favoritos", getenv("USERNAME"));
#else
    snprintf(path, sizeof(path), "%s/.biblia_favoritos", getenv("HOME"));
#endif
    FILE *f = fopen(path, "a");
    if (f) { fprintf(f, "%s %d:%d\n", v->livro, v->capitulo, v->versiculo); fclose(f); }
}

static void str_lower(char *s) {
    for (; *s; s++) if (*s >= 'A' && *s <= 'Z') *s += 32;
}

static void print_preview(const char *txt, int max) {
    int len = (int)strlen(txt);
    if (len <= max) { printf("%s", txt); return; }
    for (int i = 0; i < max - 3; i++) putchar(txt[i]);
    printf("...");
}

static const char *resolver(const char *entrada) {
    for (int i = 0; ALIASES[i].alias; i++)
        if (strcmp(entrada, ALIASES[i].alias) == 0)
            return ALIASES[i].nome;
    return NULL;
}

/* retorna 1 se resolveu referencia EXATA (cap:vers) e setou *idx */
static int tentar_ref(const char *busca, int *idx) {
    char tmp[128];
    strncpy(tmp, busca, sizeof(tmp)-1); tmp[sizeof(tmp)-1] = 0;
    str_lower(tmp);

    /* só tenta se tiver espaço + número */
    char *sp = strrchr(tmp, ' ');
    if (!sp) return 0;

    char livro_tok[64] = "";
    strncpy(livro_tok, tmp, sp - tmp);
    livro_tok[sp - tmp] = 0;

    int cap = 0, vers = 0;
    if (sscanf(sp+1, "%d:%d", &cap, &vers) < 1)
        if (sscanf(sp+1, "%d", &cap) < 1) return 0;

    const char *nome = resolver(livro_tok);
    if (!nome) return 0;

    for (int i = 0; i < TOTAL; i++) {
        int lok = strcmp(biblia[i].livro, nome) == 0;
        int cok = (cap == 0 || biblia[i].capitulo == cap);
        int vok = (vers == 0 || biblia[i].versiculo == vers);
        if (lok && cok && vok) { *idx = i; return 1; }
    }
    return 0;
}

/* ── TELAS ────────────────────────────────────────────── */
static void tela_inicial() {
    printf(CLR HIDE_CUR "\n");
    print_art(HEADER, YELLOW);
    printf("\n");
    print_art(LIVRO_ART, CYAN);
    printf("\n");
    printf(DIM "  Pressione qualquer tecla para começar...\n" RESET);
    tui_getch();
}

static void tela_ajuda() {
    printf(CLR "\n");
    print_art(HEADER, YELLOW);
    printf("\n" BOLD "  Atalhos:\n\n" RESET);
    printf("  " YELLOW "j / ↓" RESET "    Próximo versículo\n");
    printf("  " YELLOW "k / ↑" RESET "    Versículo anterior\n");
    printf("  " YELLOW "/"     RESET "        Buscar\n\n");
    printf(BOLD "  Comandos de busca:\n\n" RESET);
    printf("  " CYAN "gen" RESET "              Lista todos os versículos do Gênesis\n");
    printf("  " CYAN "joao 3:16" RESET "        Vai direto para João 3:16\n");
    printf("  " CYAN "sl 23" RESET "            Lista o Salmos 23\n");
    printf("  " CYAN "palavra amor" RESET "     Busca \"amor\" em todos os versículos\n\n");
    printf("  " YELLOW "r"     RESET "        Mensagem do dia\n");
    printf("  " YELLOW "f"     RESET "        Salvar favorito\n");
    printf("  " YELLOW "?"     RESET "        Esta tela\n");
    printf("  " YELLOW "q"     RESET "        Voltar / Sair\n\n");
    printf(DIM "  Pressione qualquer tecla para voltar...\n" RESET);
    tui_getch();
}

static void tela_random(int idx) {
    const Verso *v = &biblia[idx];
    printf(CLR "\n");
    print_art(HEADER, YELLOW);
    printf("\n" YELLOW BOLD "        ✦ Mensagem do dia ✦\n" RESET "\n");
    print_art(POMBA_ART, CYAN);
    printf("\n" YELLOW BOLD "  %s %d:%d\n" RESET, v->livro, v->capitulo, v->versiculo);
    printf("  %s\n\n", v->texto);
    printf(DIM "  [r] Outro verso  [f] Salvar  [q] Voltar\n" RESET);
}

#define MAX_RES 512

static void tela_busca(int *idx_out) {
    printf(SHOW_CUR);
    tui_raw_mode(0);
    printf(CLR "\n");
    print_art(HEADER, YELLOW);
    printf("\n" YELLOW "  Buscar: " RESET);
    fflush(stdout);

    char busca[128] = "";
    if (!fgets(busca, sizeof(busca), stdin)) { tui_raw_mode(1); printf(HIDE_CUR); return; }
    busca[strcspn(busca, "\n")] = 0;
    if (strlen(busca) == 0) { tui_raw_mode(1); printf(HIDE_CUR); return; }

    /* tenta referencia exata primeiro */
    if (tentar_ref(busca, idx_out)) {
        tui_raw_mode(1);
        printf(HIDE_CUR);
        return;
    }

    /* busca por texto ou livro */
    char tmp2[128];
    strncpy(tmp2, busca, sizeof(tmp2)-1); tmp2[sizeof(tmp2)-1] = 0;
    str_lower(tmp2);

    /* detecta comando "palavra <termo>" */
    const char *termo_busca = NULL;
    char termo_buf[128] = "";
    if (strncmp(tmp2, "palavra ", 8) == 0) {
        strncpy(termo_buf, busca + 8, sizeof(termo_buf)-1);
        termo_busca = termo_buf;
    }

    const char *nome_livro = NULL;
    if (!termo_busca)
        nome_livro = resolver(tmp2);

    int res[MAX_RES];
    int total_res = 0;
    for (int i = 0; i < TOTAL && total_res < MAX_RES; i++) {
        int match = 0;
        if (termo_busca)
            match = strstr(biblia[i].texto, termo_busca) != NULL;
        else if (nome_livro)
            match = strcmp(biblia[i].livro, nome_livro) == 0;
        else
            match = strstr(biblia[i].texto, busca) != NULL;
        if (match) res[total_res++] = i;
    }

    if (total_res == 0) {
        printf(DIM "\n  Nenhum resultado para: \"%s\"\n  Pressione qualquer tecla...\n" RESET, busca);
        fflush(stdout);
        tui_raw_mode(1);
        tui_getch();
        printf(HIDE_CUR);
        return;
    }

    tui_raw_mode(1);
    printf(HIDE_CUR);

    int sel = 0;
    while (1) {
        int rows, cols;
        tui_get_size(&rows, &cols);
        int preview = cols > 50 ? cols - 30 : 20;
        int visiveis = (rows - 10) / 3;
        if (visiveis < 3) visiveis = 3;

        printf(CLR "\n");
        print_art(HEADER, YELLOW);
        printf("\n" YELLOW BOLD "  Busca: \"%s\" — %d resultado(s)\n\n" RESET, busca, total_res);

        int inicio = sel > visiveis - 1 ? sel - (visiveis - 1) : 0;
        for (int i = inicio; i < total_res && i < inicio + visiveis; i++) {
            const Verso *v = &biblia[res[i]];
            if (i == sel) {
                printf(GREEN BOLD "  > %s %d:%d\n    " RESET GREEN, v->livro, v->capitulo, v->versiculo);
                print_preview(v->texto, preview);
                printf(RESET "\n\n");
            } else {
                printf(DIM "    %s %d:%d\n    ", v->livro, v->capitulo, v->versiculo);
                print_preview(v->texto, preview);
                printf(RESET "\n\n");
            }
        }

        print_sep(cols);
        printf(DIM " [↑↓/jk] Navegar  [Enter] Ver verso  [b] Nova busca  [q] Voltar\n" RESET);

        int c = tui_getch();
        switch (c) {
            case 'j': case KEY_DOWN: if (sel < total_res - 1) sel++; break;
            case 'k': case KEY_UP:   if (sel > 0) sel--; break;
            case '\r': case '\n': case ' ':
                *idx_out = res[sel]; return;
            case 'b': case 'B':
                tela_busca(idx_out); return;
            case 'q': case 'Q': case 27: return;
        }
    }
}

/* ── LOOP PRINCIPAL ───────────────────────────────────── */
void tui_run() {
#ifdef _WIN32
    /* UTF-8 automático */
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    /* habilita cores ANSI */
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    int rows, cols;
    tui_get_size(&rows, &cols);
    tui_raw_mode(1);
    tela_inicial();

    int idx = 0;
    int modo = 0;

    while (1) {
        tui_get_size(&rows, &cols);

        if (modo == 1) {
            tela_random(idx);
        } else {
            printf(CLR "\n");
            print_art(HEADER, YELLOW);
            printf("\n");
            print_sep(cols);
            printf("\n" YELLOW BOLD "  %s %d:%d\n" RESET "  %s\n\n",
                biblia[idx].livro, biblia[idx].capitulo, biblia[idx].versiculo, biblia[idx].texto);
            print_sep(cols);
            printf("\n");
            print_rodape();
        }

        int c = tui_getch();

        if (modo == 1) {
            switch (c) {
                case 'q': case 'Q': modo = 0; break;
                case 'r': case 'R':
                    srand((unsigned int)time(NULL) + idx);
                    idx = rand() % TOTAL; break;
                case 'f': case 'F': salvar_fav(&biblia[idx]); break;
            }
        } else {
            switch (c) {
                case 'q': case 'Q':
                    printf(SHOW_CUR CLR);
                    tui_raw_mode(0);
                    return;
                case 'j': case KEY_DOWN: if (idx < TOTAL-1) idx++; break;
                case 'k': case KEY_UP:   if (idx > 0) idx--; break;
                case 'r': case 'R':
                    srand((unsigned int)time(NULL));
                    idx = rand() % TOTAL;
                    modo = 1; break;
                case 'f': case 'F':
                    salvar_fav(&biblia[idx]);
                    printf(YELLOW "\n  ✓ Salvo: %s %d:%d\n" RESET,
                        biblia[idx].livro, biblia[idx].capitulo, biblia[idx].versiculo);
                    fflush(stdout);
#ifdef _WIN32
                    Sleep(800);
#else
                    usleep(800000);
#endif
                    break;
                case '?': tela_ajuda(); break;
                case '/': tela_busca(&idx); modo = 0; break;
            }
        }
    }
}