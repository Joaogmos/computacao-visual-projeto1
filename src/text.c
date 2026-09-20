#include "text.h"

#include <stdio.h>
#include <string.h>

TTF_Font *text_load_font(float point_size) {
    const char *base_path = SDL_GetBasePath();
    if (!base_path) {
        fprintf(stderr, "Nao foi possivel determinar o diretorio do executavel: %s\n", SDL_GetError());
        return NULL;
    }

    char font_path[1024];
    SDL_snprintf(font_path, sizeof(font_path), "%sassets/fonts/DejaVuSans.ttf", base_path);

    TTF_Font *font = TTF_OpenFont(font_path, point_size);
    if (!font) {
        fprintf(stderr, "Erro ao carregar a fonte \"%s\": %s\n", font_path, SDL_GetError());
        return NULL;
    }

    return font;
}

SDL_Texture *text_render(SDL_Renderer *renderer, TTF_Font *font, const char *str, SDL_Color color) {
    if (!font || !str || str[0] == '\0') return NULL;

    SDL_Surface *surface = TTF_RenderText_Blended(font, str, strlen(str), color);
    if (!surface) {
        fprintf(stderr, "Erro ao renderizar texto \"%s\": %s\n", str, SDL_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        fprintf(stderr, "Erro ao criar textura de texto \"%s\": %s\n", str, SDL_GetError());
    }

    return texture;
}
