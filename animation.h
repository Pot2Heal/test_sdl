#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>
#include <stdio.h>

//Declaration des Fonctions
SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer);
void animateSprite(SDL_Texture* spriteSheet, SDL_Renderer* renderer, int frameWidth, int frameHeight, int numFrames, float scaleFactor, int x, int y);

void moveSprite(SDL_Rect* destRect, int windowWidth, int windowHeight, float speed, const Uint8* keyState);

#endif
