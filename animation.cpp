#include "animation.h"

// Charger une texture à partir d'un fichier BMP
SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        printf("Erreur lors du chargement de l'image %s: %s\n", filename, SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Erreur lors de la création de la texture pour %s: %s\n", filename, SDL_GetError());
    }
    return texture;
}

// Afficher l'animation d'un sprite à partir d'une feuille de sprites
void animateSprite(SDL_Texture* spriteSheet, SDL_Renderer* renderer, int frameWidth, int frameHeight, int numFrames, float scaleFactor, int x, int y) {
    static Uint32 lastFrameTime = 0;
    static int currentFrame = 0;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > lastFrameTime + 200) {
        lastFrameTime = currentTime;
        currentFrame = (currentFrame + 1) % numFrames;
    }

    SDL_Rect srcRect = { currentFrame * frameWidth, 0, frameWidth, frameHeight };
    SDL_Rect destRect = { x, y, (int)(frameWidth * scaleFactor), (int)(frameHeight * scaleFactor) };

    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &destRect);
}
