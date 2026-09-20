#include "histogram.h"

#include <math.h>
#include <stdio.h>

/* Limiares de classificacao (decisao do grupo, arbitraria mas
 * justificavel): a faixa de intensidade 0-255 e' dividida em tercos para
 * brilho, e o desvio padrao em faixas empiricas para contraste. */
#define BRIGHTNESS_DARK_MAX 85.0
#define BRIGHTNESS_LIGHT_MIN 170.0
#define CONTRAST_LOW_MAX 32.0
#define CONTRAST_HIGH_MIN 64.0

static Uint8 pixel_intensity(Uint32 pixel, const SDL_PixelFormatDetails *details) {
    Uint8 r, g, b, a;
    SDL_GetRGBA(pixel, details, NULL, &r, &g, &b, &a);
    (void)g;
    (void)b;
    (void)a;
    return r; /* imagem de trabalho e' sempre R=G=B (escala de cinza) */
}

void histogram_compute(SDL_Surface *gray_surface, Histogram *out) {
    SDL_zerop(out);

    const SDL_PixelFormatDetails *details = SDL_GetPixelFormatDetails(gray_surface->format);
    if (SDL_MUSTLOCK(gray_surface)) SDL_LockSurface(gray_surface);

    Uint8 *pixels = (Uint8 *)gray_surface->pixels;
    Uint64 total_pixels = (Uint64)gray_surface->w * (Uint64)gray_surface->h;
    Uint64 sum = 0;

    for (int y = 0; y < gray_surface->h; y++) {
        Uint8 *row = pixels + (size_t)y * gray_surface->pitch;
        for (int x = 0; x < gray_surface->w; x++) {
            Uint32 *pixel = (Uint32 *)(row + (size_t)x * details->bytes_per_pixel);
            Uint8 v = pixel_intensity(*pixel, details);
            out->counts[v]++;
            sum += v;
        }
    }

    if (SDL_MUSTLOCK(gray_surface)) SDL_UnlockSurface(gray_surface);

    for (int i = 0; i < 256; i++) {
        if (out->counts[i] > out->max_count) out->max_count = out->counts[i];
    }

    out->mean = total_pixels > 0 ? (double)sum / (double)total_pixels : 0.0;

    double variance_sum = 0.0;
    for (int i = 0; i < 256; i++) {
        double diff = (double)i - out->mean;
        variance_sum += diff * diff * (double)out->counts[i];
    }
    out->stddev = total_pixels > 0 ? sqrt(variance_sum / (double)total_pixels) : 0.0;

    if (out->mean < BRIGHTNESS_DARK_MAX) {
        out->brightness_label = "escura";
    } else if (out->mean > BRIGHTNESS_LIGHT_MIN) {
        out->brightness_label = "clara";
    } else {
        out->brightness_label = "media";
    }

    if (out->stddev < CONTRAST_LOW_MAX) {
        out->contrast_label = "baixo";
    } else if (out->stddev > CONTRAST_HIGH_MIN) {
        out->contrast_label = "alto";
    } else {
        out->contrast_label = "medio";
    }
}

void histogram_draw(SDL_Renderer *renderer, const Histogram *hist, SDL_FRect area) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &area);

    if (hist->max_count == 0) return;

    /* Compressao logaritmica na altura das barras: evita que um pico muito
     * alto (ex. fundo uniforme) achate visualmente o resto do histograma. */
    double log_max = log(1.0 + (double)hist->max_count);
    float bar_width = area.w / 256.0f;

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    for (int i = 0; i < 256; i++) {
        if (hist->counts[i] == 0) continue;
        double log_count = log(1.0 + (double)hist->counts[i]);
        float bar_height = (float)(log_count / log_max) * area.h;

        SDL_FRect bar;
        bar.x = area.x + (float)i * bar_width;
        bar.y = area.y + area.h - bar_height;
        bar.w = bar_width > 1.0f ? bar_width : 1.0f;
        bar.h = bar_height;
        SDL_RenderFillRect(renderer, &bar);
    }
}

SDL_Surface *histogram_equalize(SDL_Surface *gray_surface) {
    Histogram hist;
    histogram_compute(gray_surface, &hist);

    Uint64 total_pixels = (Uint64)gray_surface->w * (Uint64)gray_surface->h;
    if (total_pixels == 0) {
        return SDL_CreateSurface(gray_surface->w, gray_surface->h, SDL_PIXELFORMAT_RGBA32);
    }

    /* Funcao de distribuicao acumulada (CDF) do histograma. */
    Uint64 cdf[256];
    Uint64 running = 0;
    Uint64 cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        running += hist.counts[i];
        cdf[i] = running;
        if (cdf_min == 0 && hist.counts[i] > 0) {
            cdf_min = cdf[i];
        }
    }

    Uint8 mapping[256];
    Uint64 denom = total_pixels - cdf_min;
    for (int i = 0; i < 256; i++) {
        if (denom == 0) {
            /* Imagem com um unico tom: nao ha' o que redistribuir. */
            mapping[i] = (Uint8)i;
            continue;
        }
        double scaled = ((double)cdf[i] - (double)cdf_min) / (double)denom * 255.0;
        if (scaled < 0.0) scaled = 0.0;
        if (scaled > 255.0) scaled = 255.0;
        mapping[i] = (Uint8)(scaled + 0.5);
    }

    SDL_Surface *result = SDL_CreateSurface(gray_surface->w, gray_surface->h, SDL_PIXELFORMAT_RGBA32);
    if (!result) {
        fprintf(stderr, "Erro ao alocar superficie equalizada: %s\n", SDL_GetError());
        return NULL;
    }

    const SDL_PixelFormatDetails *src_details = SDL_GetPixelFormatDetails(gray_surface->format);
    const SDL_PixelFormatDetails *dst_details = SDL_GetPixelFormatDetails(result->format);

    if (SDL_MUSTLOCK(gray_surface)) SDL_LockSurface(gray_surface);
    if (SDL_MUSTLOCK(result)) SDL_LockSurface(result);

    Uint8 *src_pixels = (Uint8 *)gray_surface->pixels;
    Uint8 *dst_pixels = (Uint8 *)result->pixels;

    for (int y = 0; y < gray_surface->h; y++) {
        Uint8 *src_row = src_pixels + (size_t)y * gray_surface->pitch;
        Uint8 *dst_row = dst_pixels + (size_t)y * result->pitch;
        for (int x = 0; x < gray_surface->w; x++) {
            Uint32 *src_pixel = (Uint32 *)(src_row + (size_t)x * src_details->bytes_per_pixel);
            Uint8 r, g, b, a;
            SDL_GetRGBA(*src_pixel, src_details, NULL, &r, &g, &b, &a);
            Uint8 new_v = mapping[r];

            Uint32 *dst_pixel = (Uint32 *)(dst_row + (size_t)x * dst_details->bytes_per_pixel);
            *dst_pixel = SDL_MapRGBA(dst_details, NULL, new_v, new_v, new_v, a);
        }
    }

    if (SDL_MUSTLOCK(result)) SDL_UnlockSurface(result);
    if (SDL_MUSTLOCK(gray_surface)) SDL_UnlockSurface(gray_surface);

    return result;
}
