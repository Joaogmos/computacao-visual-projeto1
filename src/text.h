#ifndef TEXT_H
#define TEXT_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

/* Carrega a fonte embutida em assets/fonts/DejaVuSans.ttf a partir de um
 * caminho relativo ao executavel (SDL_GetBasePath()), para que o programa
 * funcione da mesma forma em qualquer sistema operacional. */
TTF_Font *text_load_font(float point_size);

/* Renderiza uma string como textura pronta para SDL_RenderTexture(). O
 * chamador e' responsavel por destruir a textura devolvida. Devolve NULL
 * em caso de erro. */
SDL_Texture *text_render(SDL_Renderer *renderer, TTF_Font *font, const char *str, SDL_Color color);

#endif /* TEXT_H */
