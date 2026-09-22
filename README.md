# Projeto 1 - Processamento de Imagens em C com SDL3

Projeto da disciplina de **Computação Visual** (Prof. André Kishimoto),
Ciência da Computação, Faculdade de Computação e Informática, Universidade
Presbiteriana Mackenzie, 2026.2.

## Grupo

| Nome completo | RA |
|---|---|
| João Guilherme Messias de Oliveira Santos | 10426110 |
| Luis Felipe Cunha | 10419514 |

## O que é o projeto

Um programa de linha de comando, escrito em **C (C17)** com **SDL3**, que carrega
uma imagem passada como argumento e oferece operações básicas de processamento:

```
programa caminho_da_imagem.ext
```

Funcionalidades implementadas:

1. Carregamento de imagem via SDL_image, com tratamento de erros.
2. Detecção de imagem colorida × escala de cinza e conversão pela fórmula
   `Y = 0.2125·R + 0.7154·G + 0.0721·B`.
3. Interface com duas janelas: a principal exibe a imagem processada; a
   secundária (janela filha) exibe o histograma, as informações de análise e os
   botões de ação.
4. Histograma da imagem com média de intensidade (clara / média / escura) e
   desvio padrão (contraste alto / médio / baixo).
5. Equalização do histograma, reversível para a versão original em escala de
   cinza sem recarregar o arquivo.
6. Alternância entre a resolução original da imagem e 1024x768.
7. Salvar a imagem exibida em `output_image.png` com a tecla `S`.
8. Textos (histograma, informações e botões) via SDL_ttf, com a fonte
   [DejaVu Sans](assets/fonts/) embutida no repositório e carregada por
   caminho relativo ao executável.
9. Atalhos de teclado adicionais: `E` alterna equalizar/ver original e `R`
   alterna a resolução, reaproveitando a mesma ação dos botões.

## Estado atual

**Etapa 2, análise final e implementação.** O programa está implementado em
C/SDL3, com as 8 funcionalidades obrigatórias do enunciado (carregamento,
escala de cinza, duas janelas, histograma com análise, equalização
reversível, alternância de resolução, salvar com `S` e textos via SDL_ttf).

- [Relatório da Etapa 1 (Markdown)](relatorio/RELATORIO_ETAPA1.md) · [PDF](relatorio/RELATORIO_ETAPA1.pdf)
- [Relatório da Etapa 2 (Markdown)](relatorio/RELATORIO_ETAPA2.md) · [PDF](relatorio/RELATORIO_ETAPA2.pdf)
- [Enunciado do projeto](docs/enunciado-proj1.pdf)

## Ambiente de desenvolvimento

| Item | Configuração | Versão |
|---|---|---|
| Sistema operacional | Windows 11 Pro | 10.0.26200 |
| Compilador | gcc (MinGW-w64 UCRT-posix-seh) | 14.2.0 |
| Build | mingw32-make | n/a |
| SDL | SDL3 | 3.4.16 |
| SDL_image | SDL3_image | 3.4.6 |
| SDL_ttf | SDL3_ttf | 3.2.2 |

> A versão do gcc usada para o desenvolvimento (14.2.0) é anterior à 15.1.0
> citada no enunciado como a versão de compilação na correção. O `Makefile`
> fixa `-std=c17` e compila com `-Wall -Wextra` sem avisos, o que reduz o
> risco de incompatibilidade, mas vale recompilar com o gcc 15.x antes da
> entrega para confirmar. Ver seção "Problemas encontrados" do
> [relatório da Etapa 2](relatorio/RELATORIO_ETAPA2.md).

## Compilação e execução

O `Makefile` detecta automaticamente se está rodando no Windows ou no
Linux/WSL (variável `$(OS)` do make) e ajusta comandos de shell e a forma de
localizar a SDL3 de acordo. As instruções abaixo cobrem os dois casos.

### Windows

#### 1. Pré-requisitos

- **gcc** via MinGW-w64 (toolchain UCRT), com `gcc` e `mingw32-make` no `PATH`.
  Verifique com:
  ```
  gcc --version
  mingw32-make --version
  ```
  Se não tiver, instale o [WinLibs UCRT](https://winlibs.com/) (runtime UCRT,
  threads POSIX, exceções SEH) e adicione a pasta `bin` ao `PATH`.

#### 2. Bibliotecas SDL3 (não versionadas no repositório)

O `Makefile` espera as bibliotecas extraídas em `libs/`, nos caminhos:

```
libs/SDL3-3.4.16/x86_64-w64-mingw32/
libs/SDL3_image-3.4.6/x86_64-w64-mingw32/
libs/SDL3_ttf-3.2.2/x86_64-w64-mingw32/
```

Baixe os pacotes `-devel-<versão>-mingw.zip` das páginas de releases oficiais
e extraia cada um deles dentro de `libs/` (a pasta `x86_64-w64-mingw32` de
cada zip já preserva essa estrutura):

- SDL3: <https://github.com/libsdl-org/SDL/releases>
- SDL3_image (linha 3.x, não a 2.8.x): <https://github.com/libsdl-org/SDL_image/releases>
- SDL3_ttf: <https://github.com/libsdl-org/SDL_ttf/releases>

Se usar uma versão diferente das listadas acima, ajuste as variáveis
`SDL3_DIR`, `SDL3_IMAGE_DIR` e `SDL3_TTF_DIR` no topo do `Makefile`.

#### 3. Compilar

Na raiz do repositório, em qualquer terminal (PowerShell, cmd ou Git Bash):

```
mingw32-make
```

Isso compila os `.c` de `src/` para `build/*.o`, linka o executável em
`build/proj1.exe` e copia as DLLs (`SDL3.dll`, `SDL3_image.dll`,
`SDL3_ttf.dll`) e a fonte (`assets/fonts/DejaVuSans.ttf`) para dentro de
`build/`, porque no Windows as DLLs precisam estar junto do executável e o
programa procura a fonte por caminho relativo ao próprio executável.

#### 4. Executar

```
cd build
proj1.exe caminho_da_imagem.ext
```

Pressione `S` com a janela principal em foco para salvar a imagem
atualmente exibida em `build/output_image.png`, `E` para equalizar/reverter
e `R` para alternar a resolução (atalhos equivalentes aos botões).

#### 5. Limpar

```
mingw32-make clean
```

Remove a pasta `build/` inteira (objetos, executável, DLLs copiadas e a
cópia da fonte).

### Linux / WSL (Ubuntu)

O enunciado informa que o projeto também será compilado no WSL Ubuntu com
gcc 15.2.0. Nesse caso o `Makefile` não usa os pacotes `devel-mingw`, que
são específicos do Windows: ele procura a SDL3/SDL3_image/SDL3_ttf já
instaladas no sistema, via `pkg-config`.

> **Atenção:** este caminho não pôde ser testado de ponta a ponta durante o
> desenvolvimento, já que o ambiente de desenvolvimento é Windows, sem WSL
> instalado (ver relatório da Etapa 1, seção 4). Ele foi escrito seguindo o
> padrão de instalação oficial da SDL3 em Linux, mas precisa ser validado
> numa máquina Ubuntu real antes da entrega, idealmente pelo grupo.

1. Instalar dependências de build:
   ```
   sudo apt update
   sudo apt install build-essential cmake git pkg-config libx11-dev \
       libxext-dev libwayland-dev libxrandr-dev libxi-dev
   ```
2. Compilar e instalar a SDL3, SDL3_image e SDL3_ttf a partir do código-fonte
   oficial, repetindo para cada uma das três na mesma ordem, já que
   `SDL_image` e `SDL_ttf` dependem da `SDL` já instalada:
   ```
   git clone --branch release-3.4.16 https://github.com/libsdl-org/SDL.git
   cmake -S SDL -B SDL/build -DCMAKE_BUILD_TYPE=Release
   cmake --build SDL/build --parallel
   sudo cmake --install SDL/build

   git clone --branch release-3.4.6 https://github.com/libsdl-org/SDL_image.git
   cmake -S SDL_image -B SDL_image/build -DCMAKE_BUILD_TYPE=Release
   cmake --build SDL_image/build --parallel
   sudo cmake --install SDL_image/build

   git clone --branch release-3.2.2 https://github.com/libsdl-org/SDL_ttf.git
   cmake -S SDL_ttf -B SDL_ttf/build -DCMAKE_BUILD_TYPE=Release
   cmake --build SDL_ttf/build --parallel
   sudo cmake --install SDL_ttf/build

   sudo ldconfig
   ```
3. Confirmar que o `pkg-config` encontra as três bibliotecas:
   ```
   pkg-config --modversion sdl3 SDL3_image SDL3_ttf
   ```
4. Compilar e executar o projeto normalmente:
   ```
   make
   cd build
   ./proj1.exe caminho_da_imagem.ext
   ```

## Contribuições

| Integrante | Contribuição |
|---|---|
| João Guilherme Messias de Oliveira Santos | Leitura do enunciado, levantamento das versões das bibliotecas, redação do relatório da Etapa 1; condução completa da Etapa 2, incluindo implementação em C/SDL3 (carregamento, escala de cinza, histograma, equalização, janelas, botões, atalhos de teclado, Makefile), testes de compilação/execução, documentação e relatório final. |
| Luis Felipe Cunha | Leitura do enunciado, definição do processo de build e revisão do relatório da Etapa 1; colaborador do repositório, com acesso para revisar o código e a documentação da Etapa 2. |

## Estrutura do repositório

```
.
├── assets/
│   └── fonts/                # DejaVuSans.ttf (licenca livre) + LICENSE.txt
├── docs/                      # enunciado e material de apoio
├── relatorio/                 # relatórios das etapas (Markdown + PDF)
├── src/                       # código-fonte em C
│   ├── app.c / app.h          # estado da aplicação, janelas, loop de eventos
│   ├── button.c / button.h    # botões desenhados com primitivas SDL
│   ├── histogram.c / histogram.h  # histograma, estatísticas, equalização
│   ├── image.c / image.h      # carregamento, deteccão de cor, escala de cinza
│   ├── main.c                 # ponto de entrada
│   └── text.c / text.h        # renderização de texto via SDL_ttf
├── Makefile
├── .gitattributes
├── .gitignore
└── README.md
```

`libs/` (bibliotecas SDL3 baixadas) e `build/` (artefatos de compilação) não
são versionados, ver seção "Compilação e execução".
