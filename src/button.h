#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
    BUTTON_STATE_NEUTRAL = 0,
    BUTTON_STATE_HOVER,
    BUTTON_STATE_PRESSED
} ButtonState;

typedef struct {
    SDL_FRect rect;
    ButtonState state;
    const char *label;
} Button;

/* Testa se o ponto (x, y) esta' dentro do retangulo do botao. */
bool button_contains(const Button *button, float x, float y);

/* Desenha o botao com primitivas SDL (retangulo preenchido + borda) e o
 * texto do rotulo centralizado, usando a fonte informada. As cores variam
 * conforme o estado: azul neutro, azul claro (hover) e azul escuro
 * (pressionado). */
void button_draw(SDL_Renderer *renderer, TTF_Font *font, const Button *button);

#endif /* BUTTON_H */
