#include <SDL3/SDL.h>
#include <stdio.h>

#include "app.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s caminho_da_imagem.ext\n", argv[0]);
        return 1;
    }

    AppState app;
    if (!app_init(&app, argv[1])) {
        app_shutdown(&app);
        return 1;
    }

    while (app.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app_handle_event(&app, &event);
        }
        app_render(&app);
    }

    app_shutdown(&app);
    return 0;
}
