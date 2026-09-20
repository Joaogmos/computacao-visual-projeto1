#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/* Carrega uma imagem do disco, converte para o formato de trabalho fixo
 * (SDL_PIXELFORMAT_RGBA32) e devolve a superficie pronta para uso.
 * Retorna NULL em caso de erro (arquivo nao encontrado, formato invalido,
 * etc.); a mensagem de erro correspondente ja e' impressa no terminal. */
SDL_Surface *image_load(const char *path);

/* Verifica se a imagem (ja convertida para RGBA32) e' colorida ou se e'
 * uma imagem em escala de cinza (R == G == B em todos os pixels). */
bool image_is_grayscale(SDL_Surface *surface);

/* Cria uma nova superficie em escala de cinza a partir de uma superficie
 * RGBA32 usando a formula Y = 0.2125*R + 0.7154*G + 0.0721*B. O resultado
 * continua em RGBA32 (R=G=B=Y), para poder ser desenhado/salvo sem
 * conversao extra. O canal alfa original e' preservado. */
SDL_Surface *image_to_grayscale(SDL_Surface *surface);

#endif /* IMAGE_H */
