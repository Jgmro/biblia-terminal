<div align="center">

```
╔═══════════════════════════╗
║    ✝  B Í B L I A  ✝     ║
║      IC  ·  XC  ·  NIKA  ║
╚═══════════════════════════╝
```

# Bíblia de Jerusalém — Terminal

**Bíblia de Jerusalém completa em português, direto no terminal.**  
Um binário só. Sem dependências. Sem internet. Sem instalação.

Inspirado no [kjv](https://github.com/layeh/kjv) — filosofia Unix pura.

</div>

## Download direto

Não quer compilar? Baixe o binário na página de [Releases](https://github.com/Jgmro/biblia-terminal/releases).

- **Windows:** baixa `biblia.exe` e roda direto no terminal
- **Linux:** baixa `biblia`, dá permissão e instala:
```bash
chmod +x biblia
sudo cp biblia /usr/local/bin/
```
> **Windows:** o Windows Defender pode bloquear o binário por não ter assinatura digital.
> Isso é normal para executáveis open source. Para executar:
> clica em **Mais informações** → **Executar assim mesmo**.
> Se preferir, compile você mesmo seguindo as instruções abaixo.
---

## Demo

```
$ biblia joao 3:16
João 3:16
Pois Deus amou tanto o mundo, que entregou o seu Filho único,
para que todo o que nele crê não pereça, mas tenha vida eterna.

$ biblia /nao temas
Gênesis 15:1
Depois dessas coisas, o Senhor dirigiu-se a Abrão em visão:
"Não temas, Abrão! Eu sou o teu escudo..."
--- 83 resultado(s) ---

$ biblia random
Salmos 46:2
Deus é para nós refúgio e força,
socorro sempre à mão nas tribulações.
```

---

## Interface TUI

```
$ biblia
```

```
╔═══════════════════════════╗
║    ✝  B Í B L I A  ✝     ║
║      IC  ·  XC  ·  NIKA  ║
╚═══════════════════════════╝

────────────────────────────────────────────────────────────

  João 3:16
  Pois Deus amou tanto o mundo, que entregou o seu Filho único...

────────────────────────────────────────────────────────────

 [↑↓/jk] Scroll  [/] Buscar  [r] Aleatório
 [f] Favorito     [?] Ajuda   [q] Sair
```

---

## Uso

### CLI

```bash
# Busca por referência
biblia joao 3:16
biblia sl 23
biblia gn 1

# Abreviações funcionam
biblia mt 5:3
biblia ap 22:20

# Busca por texto
biblia /amor
biblia /nao temas

# Verso aleatório
biblia random

# Favoritos
biblia fav joao 3:16    # salva
biblia fav              # lista todos
```

### TUI — Atalhos

| Tecla | Ação |
|-------|------|
| `j / ↓` | Próximo versículo |
| `k / ↑` | Versículo anterior |
| `/` | Buscar |
| `r` | Mensagem do dia |
| `f` | Salvar favorito |
| `?` | Ajuda |
| `q` | Voltar / Sair |

### Comandos de busca (dentro da TUI)

| Comando | Resultado |
|---------|-----------|
| `gen` | Lista todos os versículos do Gênesis |
| `joao 3:16` | Vai direto para João 3:16 |
| `sl 23` | Lista o Salmos 23 |
| `palavra amor` | Busca "amor" em todos os versículos |

---

## Instalação

### Linux / macOS

```bash
# 1. Clona o repositório
git clone https://github.com/Jgmro/biblia-terminal
cd biblia

# 2. Compila
gcc -Wall -O2 src/main.c src/tui.c src/bible_data.c -o biblia

# 3. Instala globalmente
sudo cp biblia /usr/local/bin/

# 4. Usa de qualquer lugar
biblia joao 3:16
```

### Windows (MSYS2 UCRT64)

```bash
# 1. Clona o repositório
git clone https://github.com/seuusuario/biblia
cd biblia

# 2. Compila
gcc -Wall -O2 src/main.c src/tui.c src/bible_data.c -o biblia.exe

# 3. Copia o biblia.exe para uma pasta no PATH
#    Por exemplo: C:\Windows\System32\ (requer administrador)
#    Ou adiciona a pasta do projeto ao PATH nas variáveis de ambiente

# 4. Usa de qualquer lugar no terminal
biblia.exe joao 3:16
```

---

## Compilar

**Requisitos:** `gcc`

```bash
# Linux / macOS
gcc -Wall -O2 src/main.c src/tui.c src/bible_data.c -o biblia

# Windows (MSYS2 UCRT64)
gcc -Wall -O2 src/main.c src/tui.c src/bible_data.c -o biblia.exe
```

---

## Estrutura

```
biblia/
├── src/
│   ├── main.c          # CLI e ponto de entrada
│   ├── tui.c           # Interface interativa (TUI)
│   ├── tui.h           # Header da TUI
│   ├── bible_data.c    # 33506 versículos embedados no binário
│   └── bible_data.h    # Definição da struct Verso
├── Makefile
└── README.md
```

O texto da Bíblia está compilado diretamente no binário — nenhum arquivo externo em tempo de execução.

---

## Por que C puro?

A maioria das ferramentas de Bíblia para terminal dependem de Python, Node, banco de dados ou conexão com internet. Este projeto segue a filosofia Unix:

- **Um binário** — copia para qualquer máquina Unix e roda
- **Zero dependências** — sem runtime, sem `pip install`, sem `apt install`
- **Offline** — funciona sem internet, para sempre
- **Composível** — funciona com `grep`, `wc`, `pipes` como qualquer ferramenta Unix

```bash
# Quantas vezes "amor" aparece na Bíblia?
biblia /amor | grep -c "^[A-Z]"

# Todos os versículos de João sobre luz
biblia /luz | grep "João"
```

---

## Conteúdo

**Tradução:** Bíblia de Jerusalém em português  
**Versículos:** 33.506  
**Testamentos:** Antigo e Novo  

Livros não disponíveis na fonte original: Cântico dos Cânticos, Sabedoria, Judas.

---

## Licença

O código é MIT. O texto da Bíblia de Jerusalém é propriedade da Editora Paulus — uso pessoal.
