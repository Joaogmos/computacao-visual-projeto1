#include "app.h"

#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <string.h>

#include "image.h"
#include "text.h"

#define OUTPUT_FILE_NAME "output_image.png"

static SDL_Surface *current_surface(const AppState *app) {
    return app->showing_equalized ? app->equalized_gray : app->original_gray;
}

static void refresh_main_texture(AppState *app) {
    if (app->main_texture) {
        SDL_DestroyTexture(app->main_texture);
        app->main_texture = NULL;
    }
    SDL_Surface *surface = current_surface(app);
    app->main_texture = SDL_CreateTextureFromSurface(app->main_renderer, surface);
    if (!app->main_texture) {
        fprintf(stderr, "Erro ao criar textura da imagem: %s\n", SDL_GetError());
    }
}

static void recompute_histogram(AppState *app) {
    histogram_compute(current_surface(app), &app->histogram);
}

/* Redimensiona e reposiciona a janela principal de acordo com o modo de
 * exibicao atual (resolucao original x 1024x768), centralizando no
 * monitor principal ou fixando em (0,0) caso a imagem exceda a
 * resolucao do sistema, conforme o requisito 6 do enunciado. */
static void apply_display_mode(AppState *app) {
    SDL_Surface *surface = current_surface(app);

    int target_w, target_h;
    if (app->showing_original_resolution) {
        target_w = surface->w;
        target_h = surface->h;
    } else {
        target_w = MAIN_WINDOW_DEFAULT_W;
        target_h = MAIN_WINDOW_DEFAULT_H;
    }

    SDL_SetWindowSize(app->main_window, target_w, target_h);

    SDL_DisplayID display = SDL_GetDisplayForWindow(app->main_window);
    SDL_Rect bounds;
    bool exceeds = false;
    if (SDL_GetDisplayBounds(display, &bounds)) {
        exceeds = target_w > bounds.w || target_h > bounds.h;
    }

    if (exceeds) {
        SDL_SetWindowPosition(app->main_window, 0, 0);
    } else {
        SDL_SetWindowPosition(app->main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

static void layout_secondary_widgets(AppState *app) {
    app->equalize_button.rect = (SDL_FRect){20, 270, 320, 50};
    app->equalize_button.state = BUTTON_STATE_NEUTRAL;
    app->equalize_button.label = "Equalizar";

    app->resolution_button.rect = (SDL_FRect){20, 330, 320, 50};
    app->resolution_button.state = BUTTON_STATE_NEUTRAL;
    app->resolution_button.label = "1024x768";
}

bool app_init(AppState *app, const char *image_path) {
    memset(app, 0, sizeof(*app));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return false;
    }

    if (!TTF_Init()) {
        fprintf(stderr, "Erro ao inicializar SDL_ttf: %s\n", SDL_GetError());
        return false;
    }

    SDL_Surface *loaded = image_load(image_path);
    if (!loaded) {
        return false; /* image_load ja' imprimiu o erro */
    }

    bool already_gray = image_is_grayscale(loaded);
    printf("A imagem de entrada esta' %s.\n", already_gray ? "em escala de cinza" : "colorida");

    if (already_gray) {
        app->original_gray = loaded;
    } else {
        app->original_gray = image_to_grayscale(loaded);
        SDL_DestroySurface(loaded);
        if (!app->original_gray) return false;
        printf("Imagem convertida para escala de cinza.\n");
    }

    app->showing_equalized = false;
    app->showing_original_resolution = false;

    app->main_window = SDL_CreateWindow("Processamento de Imagens - Janela Principal",
                                         MAIN_WINDOW_DEFAULT_W, MAIN_WINDOW_DEFAULT_H,
                                         SDL_WINDOW_RESIZABLE);
    if (!app->main_window) {
        fprintf(stderr, "Erro ao criar a janela principal: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetWindowPosition(app->main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    app->main_renderer = SDL_CreateRenderer(app->main_window, NULL);
    if (!app->main_renderer) {
        fprintf(stderr, "Erro ao criar o renderer da janela principal: %s\n", SDL_GetError());
        return false;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, app->main_window);
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Histograma e Controles");
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 0);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 0);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, SECONDARY_WINDOW_W);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, SECONDARY_WINDOW_H);
    app->secondary_window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (!app->secondary_window) {
        fprintf(stderr, "Erro ao criar a janela secundaria: %s\n", SDL_GetError());
        return false;
    }

    app->secondary_renderer = SDL_CreateRenderer(app->secondary_window, NULL);
    if (!app->secondary_renderer) {
        fprintf(stderr, "Erro ao criar o renderer da janela secundaria: %s\n", SDL_GetError());
        return false;
    }

    app->font_ui = text_load_font(18.0f);
    app->font_info = text_load_font(16.0f);
    if (!app->font_ui || !app->font_info) {
        return false;
    }

    layout_secondary_widgets(app);
    refresh_main_texture(app);
    recompute_histogram(app);

    app->running = true;
    return true;
}

static void handle_button_click(AppState *app, Button *button) {
    if (button == &app->equalize_button) {
        app->showing_equalized = !app->showing_equalized;
        if (app->showing_equalized && !app->equalized_gray) {
            app->equalized_gray = histogram_equalize(app->original_gray);
        }
        app->equalize_button.label = app->showing_equalized ? "Ver original" : "Equalizar";
        refresh_main_texture(app);
        recompute_histogram(app);
        apply_display_mode(app);
    } else if (button == &app->resolution_button) {
        app->showing_original_resolution = !app->showing_original_resolution;
        app->resolution_button.label = app->showing_original_resolution ? "1024x768" : "Resolucao original";
        apply_display_mode(app);
    }
}

static void save_current_image(AppState *app) {
    SDL_PathInfo info;
    bool existed = SDL_GetPathInfo(OUTPUT_FILE_NAME, &info);

    SDL_Surface *surface = current_surface(app);
    if (IMG_SavePNG(surface, OUTPUT_FILE_NAME)) {
        printf("Arquivo %s %s.\n", OUTPUT_FILE_NAME, existed ? "sobrescrito" : "criado");
    } else {
        fprintf(stderr, "Erro ao salvar %s: %s\n", OUTPUT_FILE_NAME, SDL_GetError());
    }
}

void app_handle_event(AppState *app, const SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            app->running = false;
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            app->running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_S) {
                save_current_image(app);
            }
            break;

        case SDL_EVENT_MOUSE_MOTION: {
            if (event->motion.windowID != SDL_GetWindowID(app->secondary_window)) break;
            Button *buttons[2] = {&app->equalize_button, &app->resolution_button};
            for (int i = 0; i < 2; i++) {
                Button *b = buttons[i];
                if (b->state == BUTTON_STATE_PRESSED) continue;
                bool inside = button_contains(b, event->motion.x, event->motion.y);
                b->state = inside ? BUTTON_STATE_HOVER : BUTTON_STATE_NEUTRAL;
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if (event->button.windowID != SDL_GetWindowID(app->secondary_window)) break;
            if (event->button.button != SDL_BUTTON_LEFT) break;
            Button *buttons[2] = {&app->equalize_button, &app->resolution_button};
            for (int i = 0; i < 2; i++) {
                Button *b = buttons[i];
                if (button_contains(b, event->button.x, event->button.y)) {
                    b->state = BUTTON_STATE_PRESSED;
                }
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP: {
            if (event->button.windowID != SDL_GetWindowID(app->secondary_window)) break;
            if (event->button.button != SDL_BUTTON_LEFT) break;
            Button *buttons[2] = {&app->equalize_button, &app->resolution_button};
            for (int i = 0; i < 2; i++) {
                Button *b = buttons[i];
                bool was_pressed = (b->state == BUTTON_STATE_PRESSED);
                bool inside = button_contains(b, event->button.x, event->button.y);
                b->state = inside ? BUTTON_STATE_HOVER : BUTTON_STATE_NEUTRAL;
                if (was_pressed && inside) {
                    handle_button_click(app, b);
                }
            }
            break;
        }

        default:
            break;
    }
}

static void render_secondary_window(AppState *app) {
    SDL_SetRenderDrawColor(app->secondary_renderer, 35, 35, 40, 255);
    SDL_RenderClear(app->secondary_renderer);

    SDL_FRect hist_area = {20, 20, 320, 180};
    histogram_draw(app->secondary_renderer, &app->histogram, hist_area);

    char line1[128];
    char line2[128];
    SDL_snprintf(line1, sizeof(line1), "Media de intensidade: %.1f (%s)",
                 app->histogram.mean, app->histogram.brightness_label);
    SDL_snprintf(line2, sizeof(line2), "Desvio padrao: %.1f (contraste %s)",
                 app->histogram.stddev, app->histogram.contrast_label);

    SDL_Color info_color = {230, 230, 230, 255};
    SDL_Texture *t1 = text_render(app->secondary_renderer, app->font_info, line1, info_color);
    SDL_Texture *t2 = text_render(app->secondary_renderer, app->font_info, line2, info_color);

    if (t1) {
        float w, h;
        SDL_GetTextureSize(t1, &w, &h);
        SDL_FRect dst = {20, 210, w, h};
        SDL_RenderTexture(app->secondary_renderer, t1, NULL, &dst);
        SDL_DestroyTexture(t1);
    }
    if (t2) {
        float w, h;
        SDL_GetTextureSize(t2, &w, &h);
        SDL_FRect dst = {20, 235, w, h};
        SDL_RenderTexture(app->secondary_renderer, t2, NULL, &dst);
        SDL_DestroyTexture(t2);
    }

    button_draw(app->secondary_renderer, app->font_ui, &app->equalize_button);
    button_draw(app->secondary_renderer, app->font_ui, &app->resolution_button);

    SDL_Texture *hint = text_render(app->secondary_renderer, app->font_info,
                                     "Pressione S para salvar output_image.png", info_color);
    if (hint) {
        float w, h;
        SDL_GetTextureSize(hint, &w, &h);
        SDL_FRect dst = {20, 390, w, h};
        SDL_RenderTexture(app->secondary_renderer, hint, NULL, &dst);
        SDL_DestroyTexture(hint);
    }

    SDL_RenderPresent(app->secondary_renderer);
}

static void render_main_window(AppState *app) {
    SDL_SetRenderDrawColor(app->main_renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->main_renderer);

    if (app->main_texture) {
        int win_w, win_h;
        SDL_GetWindowSize(app->main_window, &win_w, &win_h);
        SDL_FRect dst = {0, 0, (float)win_w, (float)win_h};
        SDL_RenderTexture(app->main_renderer, app->main_texture, NULL, &dst);
    }

    SDL_RenderPresent(app->main_renderer);
}

void app_render(AppState *app) {
    render_main_window(app);
    render_secondary_window(app);
}

void app_shutdown(AppState *app) {
    if (app->main_texture) SDL_DestroyTexture(app->main_texture);
    if (app->equalized_gray) SDL_DestroySurface(app->equalized_gray);
    if (app->original_gray) SDL_DestroySurface(app->original_gray);

    if (app->font_ui) TTF_CloseFont(app->font_ui);
    if (app->font_info) TTF_CloseFont(app->font_info);

    if (app->secondary_renderer) SDL_DestroyRenderer(app->secondary_renderer);
    if (app->secondary_window) SDL_DestroyWindow(app->secondary_window);
    if (app->main_renderer) SDL_DestroyRenderer(app->main_renderer);
    if (app->main_window) SDL_DestroyWindow(app->main_window);

    TTF_Quit();
    SDL_Quit();
}
