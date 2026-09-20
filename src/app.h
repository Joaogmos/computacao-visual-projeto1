#ifndef APP_H
#define APP_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "button.h"
#include "histogram.h"

#define MAIN_WINDOW_DEFAULT_W 1024
#define MAIN_WINDOW_DEFAULT_H 768

#define SECONDARY_WINDOW_W 360
#define SECONDARY_WINDOW_H 420

typedef struct {
    /* Imagens em memoria: a original em escala de cinza nunca e' liberada
     * ate' o fim do programa; a equalizada e' calculada sob demanda e
     * fica em cache ate' a imagem original mudar. */
    SDL_Surface *original_gray;
    SDL_Surface *equalized_gray;
    bool showing_equalized;
    bool showing_original_resolution;

    SDL_Window *main_window;
    SDL_Renderer *main_renderer;
    SDL_Texture *main_texture;

    SDL_Window *secondary_window;
    SDL_Renderer *secondary_renderer;

    TTF_Font *font_ui;
    TTF_Font *font_info;

    Button equalize_button;
    Button resolution_button;

    Histogram histogram;
    bool running;
} AppState;

/* Inicializa SDL/SDL_image/SDL_ttf, carrega a imagem, cria as duas
 * janelas e prepara o estado inicial. Devolve false em caso de falha
 * (mensagens de erro ja' impressas no terminal). */
bool app_init(AppState *app, const char *image_path);

/* Trata um evento SDL, atualizando o estado da aplicacao. */
void app_handle_event(AppState *app, const SDL_Event *event);

/* Redesenha as duas janelas com o estado atual. */
void app_render(AppState *app);

/* Libera todos os recursos e finaliza as bibliotecas SDL. */
void app_shutdown(AppState *app);

#endif /* APP_H */
