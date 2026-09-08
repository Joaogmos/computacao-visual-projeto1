# Projeto 1 — Processamento de Imagens em C com SDL3

Projeto da disciplina de **Computação Visual** (Prof. André Kishimoto) —
Ciência da Computação, Faculdade de Computação e Informática, Universidade
Presbiteriana Mackenzie, 2026.2.

## Grupo

| Nome completo | RA |
|---|---|
| João Guilherme Messias de Oliveira Santos | 10426110 |

## O que é o projeto

Um programa de linha de comando, escrito em **C (C17)** com **SDL3**, que carrega
uma imagem passada como argumento e oferece operações básicas de processamento:

```
programa caminho_da_imagem.ext
```

Funcionalidades previstas:

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

## Estado atual

**Etapa 1 — Análise inicial.** O código ainda não foi implementado; esta entrega
é o relatório de levantamento de dificuldades e de configuração de ambiente.

- [Relatório da Etapa 1 (Markdown)](relatorio/RELATORIO_ETAPA1.md)
- [Relatório da Etapa 1 (PDF)](relatorio/RELATORIO_ETAPA1.pdf)
- [Enunciado do projeto](docs/enunciado-proj1.pdf)

## Ambiente de desenvolvimento

| Item | Configuração | Versão |
|---|---|---|
| Sistema operacional | Windows 11 Pro | 10.0.26200 |
| Compilador (alvo) | gcc — MinGW-w64 UCRT (WinLibs) | 15.1.0 |
| Editor | Visual Studio Code | 1.135.0 |
| SDL | SDL3 | 3.4.16 |
| SDL_image | SDL3_image | 3.4.6 |
| SDL_ttf | SDL3_ttf | 3.2.2 |
| Validação secundária | WSL Ubuntu 26.04 + gcc | 15.2.0 |

## Compilação e execução

> Esta seção será preenchida na Etapa 2, quando o `Makefile` e o código-fonte
> estiverem no repositório. O processo planejado está descrito na seção 3 do
> relatório da Etapa 1.

## Contribuições

| Integrante | Contribuição |
|---|---|
| João Guilherme Messias de Oliveira Santos | Levantamento de versões das bibliotecas, definição do processo de build e redação do relatório da Etapa 1. |

## Estrutura do repositório

```
projeto1/
├── docs/                    # enunciado e material de apoio
├── relatorio/               # relatórios das etapas (Markdown + PDF)
├── .gitignore
└── README.md
```
