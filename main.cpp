#include <SDL2/SDL.h>
#include "animation.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("DragonBouleZ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* spriteSheet = loadTexture("sprite.bmp", renderer);
    if (!spriteSheet) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    int frameWidth = 32;
    int frameHeight = 32;
    int numFrames = 6;
    float scaleFactor = 2.0f;

    SDL_Rect spriteRect = { 100, 100, frameWidth, frameHeight };

    bool running = true;
    SDL_Event event;
    float speed = 0.1f; // Définir la vitesse fractionnaire

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        moveSprite(&spriteRect, 640, 480, speed, keyState);

        SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255);
        SDL_RenderClear(renderer);

        animateSprite(spriteSheet, renderer, frameWidth, frameHeight, numFrames, scaleFactor, spriteRect.x, spriteRect.y);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(spriteSheet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
