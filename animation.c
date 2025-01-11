#include "animation.h"

// Fonction pour charger une texture à partir d'un fichier BMP
SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        printf("Erreur : Impossible de charger l'image %s. SDL_Error : %s\n", filename, SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        printf("Erreur : Impossible de créer la texture. SDL_Error : %s\n", SDL_GetError());
    }

    return texture;
}

// Fonction pour animer un sprite
void animateSprite(SDL_Texture* spriteSheet, SDL_Renderer* renderer, int frameWidth, int frameHeight, int numFrames, float scaleFactor, int x, int y, bool flipHorizontal) {
    static int currentFrame = 0; // Frame actuelle
    static Uint32 lastUpdateTime = 0; // Temps du dernier changement de frame

    // Définir l'intervalle entre les frames (en millisecondes)
    const int frameDelay = 100; // Par exemple, 100 ms entre chaque frame
    Uint32 currentTime = SDL_GetTicks();

    // Vérifier si le temps pour changer de frame est écoulé
    if (currentTime > lastUpdateTime + frameDelay) {
        currentFrame = (currentFrame + 1) % numFrames; // Passer à la frame suivante
        lastUpdateTime = currentTime;
    }

    // Définir la zone de découpage dans la sprite sheet
    SDL_Rect srcRect = {
        .x = currentFrame * frameWidth,
        .y = 0,
        .w = frameWidth,
        .h = frameHeight
    };

    // Définir la zone d'affichage de la frame sur l'écran
    SDL_Rect destRect = {
        .x = x,
        .y = y,
        .w = (int)(frameWidth * scaleFactor),
        .h = (int)(frameHeight * scaleFactor)
    };

    // Définir le mode de flip si demandé
    SDL_RendererFlip flip = flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // Rendu de la texture
    if (SDL_RenderCopyEx(renderer, spriteSheet, &srcRect, &destRect, 0, NULL, flip) != 0) {
        printf("Erreur : Impossible de rendre l'animation. SDL_Error : %s\n", SDL_GetError());
    }
}
