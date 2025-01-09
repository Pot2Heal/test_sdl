#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>
#include <stdio.h>

// Déclaration des fonctions
SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer);
void animateSprite(SDL_Texture* spriteSheet, SDL_Renderer* renderer, int frameWidth, int frameHeight, int numFrames, float scaleFactor, int x, int y, bool flipHorizontal);

#endif
