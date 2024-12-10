#include "animation.h"

// Charge une texture � partir d'un fichier BMP
SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        printf("Erreur lors du chargement de l'image %s: %s\n", filename, SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Erreur lors de la cr�ation de la texture pour %s: %s\n", filename, SDL_GetError());
    }
    return texture;
}

// Affiche une animation � partir d'une feuille de sprites
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

// D�place le sprite en fonction des touches press�es
void moveSprite(SDL_Rect* destRect, int windowWidth, int windowHeight, float speed, const Uint8* keyState) {
    static float x = destRect->x;
    static float y = destRect->y;

    if (keyState[SDL_SCANCODE_W]) y -= speed;
    if (keyState[SDL_SCANCODE_S]) y += speed;
    if (keyState[SDL_SCANCODE_D]) x += speed;
    if (keyState[SDL_SCANCODE_A]) x -= speed;

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + destRect->w > windowWidth) x = windowWidth - destRect->w;
    if (y + destRect->h > windowHeight) y = windowHeight - destRect->h;

    destRect->x = (int)x;
    destRect->y = (int)y;
}
