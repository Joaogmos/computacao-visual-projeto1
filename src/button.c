#include "button.h"
#include "text.h"

bool button_contains(const Button *button, float x, float y) {
    return x >= button->rect.x && x < button->rect.x + button->rect.w &&
           y >= button->rect.y && y < button->rect.y + button->rect.h;
}

void button_draw(SDL_Renderer *renderer, TTF_Font *font, const Button *button) {
    SDL_Color fill;
    switch (button->state) {
        case BUTTON_STATE_HOVER:
            fill = (SDL_Color){110, 160, 235, 255}; /* azul claro */
            break;
        case BUTTON_STATE_PRESSED:
            fill = (SDL_Color){20, 60, 140, 255}; /* azul escuro */
            break;
        case BUTTON_STATE_NEUTRAL:
        default:
            fill = (SDL_Color){60, 110, 200, 255}; /* azul neutro */
            break;
    }

    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &button->rect);

    SDL_SetRenderDrawColor(renderer, 10, 20, 40, 255);
    SDL_RenderRect(renderer, &button->rect);

    if (font && button->label) {
        SDL_Color text_color = {255, 255, 255, 255};
        SDL_Texture *label_texture = text_render(renderer, font, button->label, text_color);
        if (label_texture) {
            float tw, th;
            SDL_GetTextureSize(label_texture, &tw, &th);
            SDL_FRect dst;
            dst.w = tw;
            dst.h = th;
            dst.x = button->rect.x + (button->rect.w - tw) / 2.0f;
            dst.y = button->rect.y + (button->rect.h - th) / 2.0f;
            SDL_RenderTexture(renderer, label_texture, NULL, &dst);
            SDL_DestroyTexture(label_texture);
        }
    }
}
