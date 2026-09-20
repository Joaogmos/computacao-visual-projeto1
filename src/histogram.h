#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <SDL3/SDL.h>

typedef struct {
    Uint32 counts[256];
    Uint32 max_count;
    double mean;
    double stddev;
    const char *brightness_label; /* "clara" / "media" / "escura" */
    const char *contrast_label;   /* "alto" / "medio" / "baixo" */
} Histogram;

/* Calcula o histograma de intensidades (canal Y, ja que a imagem de
 * trabalho esta' sempre em escala de cinza com R=G=B) e as estatisticas
 * de media/desvio padrao/classificacao. */
void histogram_compute(SDL_Surface *gray_surface, Histogram *out);

/* Desenha o histograma e o texto de analise dentro do retangulo dado, no
 * renderer da janela secundaria. */
void histogram_draw(SDL_Renderer *renderer, const Histogram *hist, SDL_FRect area);

/* Equaliza o histograma de uma imagem em escala de cinza usando a funcao
 * de distribuicao acumulada (CDF). Devolve uma nova superficie RGBA32;
 * quem chama e' responsavel por liberar. */
SDL_Surface *histogram_equalize(SDL_Surface *gray_surface);

#endif /* HISTOGRAM_H */
