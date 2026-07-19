/* 
   tui.c — interface interativa da Bíblia
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>  /* API do console do Windows */
#include <conio.h>    /* _getch() para leitura de tecla no Windows */
#else
#include <termios.h>  /* controle do terminal no Linux/macOS */
#include <unistd.h>
#include <sys/ioctl.h> /* ioctl para obter tamanho do terminal */
#endif
#include "bible_data.h"
#include "tui.h"

/* ── CÓDIGOS ANSI ─────────────────────────────────────────
   sequências de escape que controlam o terminal:
   \033 = ESC, seguido de [ e o código do comando          */
#define CLR      "\033[2J\033[H"  /* limpa a tela e move cursor pro topo */
#define HIDE_CUR "\033[?25l"      /* esconde o cursor (evita piscar na TUI) */
#define SHOW_CUR "\033[?25h"      /* mostra o cursor novamente */
#define BOLD     "\033[1m"        /* texto em negrito */
#define DIM      "\033[2m"        /* texto mais escuro (para rodapé) */
#define YELLOW   "\033[33m"       /* texto amarelo (referências e destaques) */
#define CYAN     "\033[36m"       /* texto ciano (arte ASCII) */
#define GREEN    "\033[32m"       /* texto verde (item selecionado na lista) */
#define RESET    "\033[0m"        /* reseta todas as formatações */

/* ── ALIASES DE LIVROS ────────────────────────────────────
   mesma tabela do main.c — necessária para a busca na TUI
   permite digitar "joao 3:16", "sl 23", "gn" etc.         */
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
    {NULL,NULL}  /* sentinela — marca o fim da tabela */
};

/* arte ASCII exibida na tela inicial — representa um livro/bíblia */
static const char *LIVRO_ART[] = {
    "          _______       ",
    "         /       /_    ",
    "        /  -/-  / /    ",
    "       /   /   / /     ",
    "      /_______/ /      ",
    "     ((______| /       ",
    "      `\"\"\"\"\"\"\"'        ",
    NULL  /* NULL marca o fim do array */
};

/* arte ASCII da pomba — exibida na tela de verso aleatório (Mensagem do dia)
   representa o Espírito Santo */
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

/* header exibido no topo de todas as telas
   IC XC NIKA = "Jesus Cristo Vence" em grego */
static const char *HEADER[] = {
    "╔═══════════════════════════╗",
    "║    ✝  B Í B L I A  ✝     ║",
    "║      IC  ·  XC  ·  NIKA  ║",
    "╚═══════════════════════════╝",
    NULL
};

/* ── CONTROLE DO TERMINAL ─────────────────────────────────
   implementações separadas para Windows e Linux/macOS     */
#ifdef _WIN32
static DWORD orig_mode;  /* salva o modo original do console para restaurar ao sair */

/* ativa/desativa o modo raw do console do Windows
   modo raw: lê tecla por tecla sem esperar Enter, sem eco no terminal */
void tui_raw_mode(int enable) {
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    if (enable) {
        GetConsoleMode(h, &orig_mode);
        /* remove ENABLE_LINE_INPUT (aguardar Enter) e ENABLE_ECHO_INPUT (mostrar teclas) */
        SetConsoleMode(h, orig_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    } else {
        SetConsoleMode(h, orig_mode);  /* restaura o modo original */
    }
}

/* lê uma tecla do teclado no Windows
   teclas especiais como setas geram dois bytes: 0 ou 224, seguido do código da tecla */
int tui_getch() {
    int c = _getch();
    if (c == 0 || c == 224) {  /* prefixo de tecla especial */
        int c2 = _getch();
        if (c2 == 72) return KEY_UP;    /* seta para cima */
        if (c2 == 80) return KEY_DOWN;  /* seta para baixo */
    }
    return c;
}

/* obtém o tamanho atual da janela do terminal no Windows */
void tui_get_size(int *rows, int *cols) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

#else
static struct termios orig_termios;  /* salva configuração original do terminal */

/* ativa/desativa o modo raw no Linux/macOS usando termios
   ICANON: desativa leitura linha a linha
   ECHO: desativa exibição das teclas digitadas */
void tui_raw_mode(int enable) {
    if (enable) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;   /* lê pelo menos 1 byte por vez */
        raw.c_cc[VTIME] = 0;  /* sem timeout */
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }
}

/* lê uma tecla no Linux/macOS
   setas chegam como sequência ESC [ A/B (3 bytes) */
int tui_getch() {
    int c = getchar();
    if (c == 27) {          /* ESC — início de sequência de seta */
        int c2 = getchar();
        if (c2 == '[') {
            int c3 = getchar();
            if (c3 == 'A') return KEY_UP;    /* ESC [ A = seta cima */
            if (c3 == 'B') return KEY_DOWN;  /* ESC [ B = seta baixo */
        }
        return 27;  /* ESC sozinho */
    }
    return c;
}

/* obtém o tamanho da janela do terminal no Linux/macOS */
void tui_get_size(int *rows, int *cols) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *rows = w.ws_row;
    *cols = w.ws_col;
}
#endif

/* ── FUNÇÕES AUXILIARES ───────────────────────────────────*/

/* imprime um array de strings linha a linha com uma cor ANSI */
static void print_art(const char **art, const char *color) {
    for (int i = 0; art[i]; i++)
        printf("%s%s%s\n", color, art[i], RESET);
}

/* imprime uma linha separadora horizontal amarela
   limitada a 60 caracteres para não ultrapassar a largura do header */
static void print_sep(int cols) {
    printf(YELLOW);
    int n = cols > 60 ? 60 : cols;
    for (int i = 0; i < n; i++) printf("─");
    printf(RESET "\n");
}

/* imprime o rodapé com os atalhos de teclado disponíveis */
static void print_rodape() {
    printf(DIM " [↑↓/jk] Scroll  [/] Buscar  [r] Aleatório\n");
    printf(" [f] Favorito     [?] Ajuda   [q] Sair\n" RESET);
}

/* salva um versículo no arquivo de favoritos
   Windows: C:\Users\<usuario>\.biblia_favoritos
   Linux/macOS: ~/.biblia_favoritos */
static void salvar_fav(const Verso *v) {
    char path[256];
#ifdef _WIN32
    snprintf(path, sizeof(path), "C:\\Users\\%s\\.biblia_favoritos", getenv("USERNAME"));
#else
    snprintf(path, sizeof(path), "%s/.biblia_favoritos", getenv("HOME"));
#endif
    FILE *f = fopen(path, "a");  /* abre em modo append — não sobrescreve */
    if (f) { fprintf(f, "%s %d:%d\n", v->livro, v->capitulo, v->versiculo); fclose(f); }
}

/* converte string para minúsculo in-place — necessário para busca case-insensitive */
static void str_lower(char *s) {
    for (; *s; s++) if (*s >= 'A' && *s <= 'Z') *s += 32;
}

/* imprime um preview do texto truncado em max caracteres
   evita que textos longos quebrem o layout da lista de resultados */
static void print_preview(const char *txt, int max) {
    int len = (int)strlen(txt);
    if (len <= max) { printf("%s", txt); return; }
    for (int i = 0; i < max - 3; i++) putchar(txt[i]);
    printf("...");  /* indica que o texto foi cortado */
}

/* resolve um alias para o nome completo do livro
   ex: "joao" → "João", "sl" → "Salmos" */
static const char *resolver(const char *entrada) {
    for (int i = 0; ALIASES[i].alias; i++)
        if (strcmp(entrada, ALIASES[i].alias) == 0)
            return ALIASES[i].nome;
    return NULL;
}

/* tenta interpretar a busca como uma referência bíblica (livro cap:vers)
   retorna 1 se encontrou e setou *idx, 0 caso contrário
   só aceita referências com número — livro sozinho vai para a lista */
static int tentar_ref(const char *busca, int *idx) {
    char tmp[128];
    strncpy(tmp, busca, sizeof(tmp)-1); tmp[sizeof(tmp)-1] = 0;
    str_lower(tmp);

    /* precisa ter espaço + número para ser uma referência */
    char *sp = strrchr(tmp, ' ');
    if (!sp) return 0;

    char livro_tok[64] = "";
    strncpy(livro_tok, tmp, sp - tmp);
    livro_tok[sp - tmp] = 0;

    int cap = 0, vers = 0;
    /* tenta "cap:vers" primeiro, depois só "cap" */
    if (sscanf(sp+1, "%d:%d", &cap, &vers) < 1)
        if (sscanf(sp+1, "%d", &cap) < 1) return 0;

    const char *nome = resolver(livro_tok);
    if (!nome) return 0;

    /* procura o primeiro versículo que bate com livro + cap + vers */
    for (int i = 0; i < TOTAL; i++) {
        int lok = strcmp(biblia[i].livro, nome) == 0;
        int cok = (cap == 0 || biblia[i].capitulo == cap);
        int vok = (vers == 0 || biblia[i].versiculo == vers);
        if (lok && cok && vok) { *idx = i; return 1; }
    }
    return 0;
}

/* ── TELAS ────────────────────────────────────────────────*/

/* tela de boas-vindas — exibida ao abrir o programa
   mostra o header e a arte do livro */
static void tela_inicial() {
    printf(CLR HIDE_CUR "\n");
    print_art(HEADER, YELLOW);
    printf("\n");
    print_art(LIVRO_ART, CYAN);
    printf("\n");
    printf(DIM "  Pressione qualquer tecla para começar...\n" RESET);
    tui_getch();  /* aguarda qualquer tecla */
}

/* tela de ajuda — exibida ao pressionar '?'
   lista todos os atalhos e comandos de busca disponíveis */
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

/* tela de verso aleatório — exibida ao pressionar 'r'
   mostra a pomba ASCII e um versículo aleatório do array */
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

/* número máximo de resultados guardados na busca por texto */
#define MAX_RES 512

/* tela de busca — exibida ao pressionar '/'
   aceita três tipos de entrada:
   1. referência: "joao 3:16" → vai direto ao verso
   2. livro: "gen" → lista todos os versículos do Gênesis
   3. palavra: "palavra amor" → busca "amor" no texto de todos os versículos */
static void tela_busca(int *idx_out) {
    /* desativa modo raw para receber input de texto normal */
    printf(SHOW_CUR);
    tui_raw_mode(0);
    printf(CLR "\n");
    print_art(HEADER, YELLOW);
    printf("\n" YELLOW "  Buscar: " RESET);
    fflush(stdout);

    char busca[128] = "";
    if (!fgets(busca, sizeof(busca), stdin)) { tui_raw_mode(1); printf(HIDE_CUR); return; }
    busca[strcspn(busca, "\n")] = 0;  /* remove o \n do final */
    if (strlen(busca) == 0) { tui_raw_mode(1); printf(HIDE_CUR); return; }

    /* tenta interpretar como referência exata (ex: "joao 3:16") */
    if (tentar_ref(busca, idx_out)) {
        tui_raw_mode(1);
        printf(HIDE_CUR);
        return;
    }

    /* prepara a string em minúsculo para comparação */
    char tmp2[128];
    strncpy(tmp2, busca, sizeof(tmp2)-1); tmp2[sizeof(tmp2)-1] = 0;
    str_lower(tmp2);

    /* detecta o comando "palavra <termo>" para busca explícita por texto */
    const char *termo_busca = NULL;
    char termo_buf[128] = "";
    if (strncmp(tmp2, "palavra ", 8) == 0) {
        strncpy(termo_buf, busca + 8, sizeof(termo_buf)-1);
        termo_busca = termo_buf;
    }

    /* se não é "palavra", tenta resolver como nome de livro */
    const char *nome_livro = NULL;
    if (!termo_busca)
        nome_livro = resolver(tmp2);

    /* coleta os índices dos versículos que correspondem à busca */
    int res[MAX_RES];
    int total_res = 0;
    for (int i = 0; i < TOTAL && total_res < MAX_RES; i++) {
        int match = 0;
        if (termo_busca)
            match = strstr(biblia[i].texto, termo_busca) != NULL;  /* busca por palavra */
        else if (nome_livro)
            match = strcmp(biblia[i].livro, nome_livro) == 0;      /* filtra por livro */
        else
            match = strstr(biblia[i].texto, busca) != NULL;        /* busca genérica */
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

    /* reativa modo raw para navegação na lista de resultados */
    tui_raw_mode(1);
    printf(HIDE_CUR);

    int sel = 0;  /* índice do item selecionado na lista */
    while (1) {
        int rows, cols;
        tui_get_size(&rows, &cols);
        int preview = cols > 50 ? cols - 30 : 20;         /* tamanho do preview do texto */
        int visiveis = (rows - 10) / 3;                   /* quantos resultados cabem na tela */
        if (visiveis < 3) visiveis = 3;

        printf(CLR "\n");
        print_art(HEADER, YELLOW);
        printf("\n" YELLOW BOLD "  Busca: \"%s\" — %d resultado(s)\n\n" RESET, busca, total_res);

        /* janela deslizante — mostra apenas os resultados ao redor do selecionado */
        int inicio = sel > visiveis - 1 ? sel - (visiveis - 1) : 0;
        for (int i = inicio; i < total_res && i < inicio + visiveis; i++) {
            const Verso *v = &biblia[res[i]];
            if (i == sel) {
                /* item selecionado — verde com seta */
                printf(GREEN BOLD "  > %s %d:%d\n    " RESET GREEN, v->livro, v->capitulo, v->versiculo);
                print_preview(v->texto, preview);
                printf(RESET "\n\n");
            } else {
                /* outros itens — cinza */
                printf(DIM "    %s %d:%d\n    ", v->livro, v->capitulo, v->versiculo);
                print_preview(v->texto, preview);
                printf(RESET "\n\n");
            }
        }

        print_sep(cols);
        printf(DIM " [↑↓/jk] Navegar  [Enter] Ver verso  [b] Nova busca  [q] Voltar\n" RESET);

        int c = tui_getch();
        switch (c) {
            case 'j': case KEY_DOWN: if (sel < total_res - 1) sel++; break;  /* desce na lista */
            case 'k': case KEY_UP:   if (sel > 0) sel--; break;              /* sobe na lista */
            case '\r': case '\n': case ' ':
                *idx_out = res[sel]; return;   /* seleciona o verso e volta ao modo normal */
            case 'b': case 'B':
                tela_busca(idx_out); return;   /* abre nova busca sem voltar ao modo normal */
            case 'q': case 'Q': case 27: return;  /* cancela e volta sem selecionar */
        }
    }
}

/* ── LOOP PRINCIPAL ───────────────────────────────────────
   controla os dois modos da TUI:
   modo 0 = normal (navega versículo a versículo)
   modo 1 = random (exibe mensagem do dia com a pomba)    */
void tui_run() {
#ifdef _WIN32
    /* configura o terminal do Windows automaticamente */
    SetConsoleOutputCP(65001);  /* UTF-8 para output — exibe acentos corretamente */
    SetConsoleCP(65001);        /* UTF-8 para input */
    /* habilita suporte a cores ANSI no Windows 10+ */
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    int rows, cols;
    tui_get_size(&rows, &cols);
    tui_raw_mode(1);   /* ativa modo raw — leitura tecla por tecla */
    tela_inicial();    /* exibe tela de boas-vindas */

    int idx = 0;   /* índice do versículo atual no array biblia[] */
    int modo = 0;  /* 0 = normal, 1 = mensagem do dia */

    while (1) {
        tui_get_size(&rows, &cols);  /* atualiza tamanho (usuário pode redimensionar a janela) */

        /* desenha a tela de acordo com o modo atual */
        if (modo == 1) {
            tela_random(idx);
        } else {
            printf(CLR "\n");
            print_art(HEADER, YELLOW);
            printf("\n");
            print_sep(cols);
            /* exibe o versículo atual com referência em amarelo e texto normal */
            printf("\n" YELLOW BOLD "  %s %d:%d\n" RESET "  %s\n\n",
                biblia[idx].livro, biblia[idx].capitulo, biblia[idx].versiculo, biblia[idx].texto);
            print_sep(cols);
            printf("\n");
            print_rodape();
        }

        int c = tui_getch();  /* aguarda input do usuário */

        if (modo == 1) {
            /* controles do modo mensagem do dia */
            switch (c) {
                case 'q': case 'Q': modo = 0; break;  /* volta ao modo normal */
                case 'r': case 'R':
                    /* gera novo verso aleatório — seed muda com idx para evitar repetição */
                    srand((unsigned int)time(NULL) + idx);
                    idx = rand() % TOTAL; break;
                case 'f': case 'F': salvar_fav(&biblia[idx]); break;  /* salva o verso atual */
            }
        } else {
            /* controles do modo normal */
            switch (c) {
                case 'q': case 'Q':
                    /* restaura o terminal antes de sair */
                    printf(SHOW_CUR CLR);
                    tui_raw_mode(0);
                    return;
                case 'j': case KEY_DOWN: if (idx < TOTAL-1) idx++; break;  /* próximo verso */
                case 'k': case KEY_UP:   if (idx > 0) idx--; break;        /* verso anterior */
                case 'r': case 'R':
                    srand((unsigned int)time(NULL));
                    idx = rand() % TOTAL;
                    modo = 1; break;  /* entra no modo mensagem do dia */
                case 'f': case 'F':
                    salvar_fav(&biblia[idx]);
                    /* feedback visual por 800ms */
                    printf(YELLOW "\n  ✓ Salvo: %s %d:%d\n" RESET,
                        biblia[idx].livro, biblia[idx].capitulo, biblia[idx].versiculo);
                    fflush(stdout);
#ifdef _WIN32
                    Sleep(800);        /* milissegundos no Windows */
#else
                    usleep(800000);    /* microsegundos no Linux/macOS */
#endif
                    break;
                case '?': tela_ajuda(); break;              /* abre tela de ajuda */
                case '/': tela_busca(&idx); modo = 0; break; /* abre busca */
            }
        }
    }
}