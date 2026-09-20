#include "image.h"

#include <SDL3_image/SDL_image.h>
#include <stdio.h>

SDL_Surface *image_load(const char *path) {
    SDL_Surface *loaded = IMG_Load(path);
    if (!loaded) {
        fprintf(stderr, "Erro ao carregar a imagem \"%s\": %s\n", path, SDL_GetError());
        return NULL;
    }

    /* Converte para um formato de pixel fixo e conhecido logo apos o
     * carregamento, para que o resto do programa nao precise lidar com a
     * variedade de formatos que SDL_image pode devolver. */
    SDL_Surface *converted = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(loaded);

    if (!converted) {
        fprintf(stderr, "Erro ao converter a imagem \"%s\" para o formato de trabalho: %s\n",
                path, SDL_GetError());
        return NULL;
    }

    printf("Imagem carregada: \"%s\" (%dx%d pixels)\n", path, converted->w, converted->h);
    return converted;
}

bool image_is_grayscale(SDL_Surface *surface) {
    const SDL_PixelFormatDetails *details = SDL_GetPixelFormatDetails(surface->format);

    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    Uint8 *pixels = (Uint8 *)surface->pixels;
    bool is_gray = true;

    for (int y = 0; y < surface->h && is_gray; y++) {
        Uint8 *row = pixels + (size_t)y * surface->pitch;
        for (int x = 0; x < surface->w; x++) {
            Uint32 *pixel = (Uint32 *)(row + (size_t)x * details->bytes_per_pixel);
            Uint8 r, g, b, a;
            SDL_GetRGBA(*pixel, details, NULL, &r, &g, &b, &a);
            if (r != g || g != b) {
                is_gray = false;
                break;
            }
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    return is_gray;
}

SDL_Surface *image_to_grayscale(SDL_Surface *surface) {
    SDL_Surface *gray = SDL_CreateSurface(surface->w, surface->h, SDL_PIXELFORMAT_RGBA32);
    if (!gray) {
        fprintf(stderr, "Erro ao alocar superficie em escala de cinza: %s\n", SDL_GetError());
        return NULL;
    }

    const SDL_PixelFormatDetails *details = SDL_GetPixelFormatDetails(surface->format);
    const SDL_PixelFormatDetails *gray_details = SDL_GetPixelFormatDetails(gray->format);

    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
    if (SDL_MUSTLOCK(gray)) SDL_LockSurface(gray);

    Uint8 *src_pixels = (Uint8 *)surface->pixels;
    Uint8 *dst_pixels = (Uint8 *)gray->pixels;

    for (int y = 0; y < surface->h; y++) {
        Uint8 *src_row = src_pixels + (size_t)y * surface->pitch;
        Uint8 *dst_row = dst_pixels + (size_t)y * gray->pitch;
        for (int x = 0; x < surface->w; x++) {
            Uint32 *src_pixel = (Uint32 *)(src_row + (size_t)x * details->bytes_per_pixel);
            Uint8 r, g, b, a;
            SDL_GetRGBA(*src_pixel, details, NULL, &r, &g, &b, &a);

            /* Formula do enunciado: Y = 0.2125*R + 0.7154*G + 0.0721*B */
            double y_value = 0.2125 * r + 0.7154 * g + 0.0721 * b;
            Uint8 y8 = (Uint8)(y_value < 0.0 ? 0.0 : (y_value > 255.0 ? 255.0 : y_value + 0.5));

            Uint32 *dst_pixel = (Uint32 *)(dst_row + (size_t)x * gray_details->bytes_per_pixel);
            *dst_pixel = SDL_MapRGBA(gray_details, NULL, y8, y8, y8, a);
        }
    }

    if (SDL_MUSTLOCK(gray)) SDL_UnlockSurface(gray);
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

    return gray;
}
