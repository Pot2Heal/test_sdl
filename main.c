#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "map.h"
#include "animation.h"
#include "gameobject.h"

typedef struct {
    int x, y, width, height;
} Rect;

// Zoom factor
const float zoomFactor = 1.6f;

// Zones interdites
Rect forbiddenZones[] = {
    {396, 268, 250, 220}, // Maison en haut a gauche    
    {872, 256, 400, 230}, // Maison en Haut a droit
    {820, 573, 550, 250}, // Maison en bas a Gauche
    {824, 1066, 500, 50}, // Barriere en Bas a Gauche
    {398, 691, 275, 25}, // Barriere en Haut a Droite
    {494, 1090, 220, 500}, // Etang
};

int isInForbiddenZone(int x, int y, Rect* zones, int numZones) {
    for (int i = 0; i < numZones; i++) {
        Rect zone = zones[i];
        if (x >= zone.x && x < zone.x + zone.width && y >= zone.y && y < zone.y + zone.height) {
            return 1;
        }
    }
    return 0;
}

void renderText(SDL_Renderer* renderer, const char* text, int x, int y) {
    SDL_Color color = { 255, 255, 255, 255 };
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Erreur chargement de la police : %s\n", TTF_GetError());
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        printf("Erreur création surface de texte : %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Erreur création texture de texte : %s\n", TTF_GetError());
    }

    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    if (TTF_Init() < 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "DragonBouleZ - Le Retour",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 960, SDL_WINDOW_SHOWN
    );
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

    GameObjectManager* objectManager = createGameObjectManager(renderer, 3);
    if (!objectManager) {
        printf("Erreur création GameObjectManager\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialiser le viewport du GameObjectManager
    objectManager->viewPortX = 0;
    objectManager->viewPortY = 0;

    Map map = loadMap("Startermap.bmp", renderer);
    if (!map.texture) {
        printf("Erreur chargement de la carte\n");
        destroyGameObjectManager(objectManager);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    int playerFrameWidth = 32, playerFrameHeight = 32;
    int playerNumFramesIdle = 4, playerNumFramesMove = 6;
    float playerScaleFactor = 3.0f;
    float playerSpeed = 200.0f;

    int playerX = map.width / 2 - playerFrameWidth / 2;
    int playerY = map.height / 2 - playerFrameHeight / 2;
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    SDL_Texture* moveSpriteSheet = loadTexture("sprite.bmp", renderer);
    SDL_Texture* idleSpriteSheet = loadTexture("spriteidle.bmp", renderer);
    if (!moveSpriteSheet || !idleSpriteSheet) {
        printf("Erreur chargement des sprites\n");
        destroyGameObjectManager(objectManager);
        SDL_DestroyTexture(map.texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* currentSpriteSheet = idleSpriteSheet;
    map.viewPort.w = (int)(1280 / zoomFactor);
    map.viewPort.h = (int)(960 / zoomFactor);

    Uint32 lastTime = SDL_GetTicks();
    Uint32 frameDelay = 16;
    int running = 1;

    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (handleRestartClick(objectManager, mouseX, mouseY)) {
                    playerX = map.width / 2 - playerFrameWidth / 2;
                    playerY = map.height / 2 - playerFrameHeight / 2;
                }
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        int moving = 0;
        int newX = playerX, newY = playerY;

        if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) {
            newY -= playerSpeed * (frameDelay / 1000.0f);
            moving = 1;
        }
        if (keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN]) {
            newY += playerSpeed * (frameDelay / 1000.0f);
            moving = 1;
        }
        if (keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) {
            newX -= playerSpeed * (frameDelay / 1000.0f);
            moving = 1;
            flip = SDL_FLIP_HORIZONTAL;
        }
        if (keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) {
            newX += playerSpeed * (frameDelay / 1000.0f);
            moving = 1;
            flip = SDL_FLIP_NONE;
        }

        if (!isInForbiddenZone(newX, newY, forbiddenZones, sizeof(forbiddenZones) / sizeof(forbiddenZones[0]))) {
            playerX = newX;
            playerY = newY;
        }

        // Update the viewport to follow the player
        map.viewPort.x = playerX - (map.viewPort.w / 2);
        map.viewPort.y = playerY - (map.viewPort.h / 2);

        // Ensure the viewport stays within the map boundaries
        if (map.viewPort.x < 0) map.viewPort.x = 0;
        if (map.viewPort.y < 0) map.viewPort.y = 0;
        if (map.viewPort.x + map.viewPort.w > map.width)
            map.viewPort.x = map.width - map.viewPort.w;
        if (map.viewPort.y + map.viewPort.h > map.height)
            map.viewPort.y = map.height - map.viewPort.h;

        // Mettre à jour le viewport des objets
        updateViewport(objectManager, map.viewPort.x, map.viewPort.y);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Rendre dans le bon ordre
        renderMap(renderer, &map);
        renderGameObjects(renderer, objectManager);

        if (moving) {
            animateSprite(moveSpriteSheet, renderer, playerFrameWidth, playerFrameHeight,
                playerNumFramesMove, playerScaleFactor,
                playerX - map.viewPort.x, playerY - map.viewPort.y, flip);
        }
        else {
            animateSprite(idleSpriteSheet, renderer, playerFrameWidth, playerFrameHeight,
                playerNumFramesIdle, playerScaleFactor,
                playerX - map.viewPort.x, playerY - map.viewPort.y, flip);
        }

        // Vérifier les collisions avec la position relative du joueur
        checkCollisions(objectManager,
            playerX - map.viewPort.x,
            playerY - map.viewPort.y,
            playerFrameWidth * playerScaleFactor,
            playerFrameHeight * playerScaleFactor);

        char playerCoordinates[50];
        snprintf(playerCoordinates, sizeof(playerCoordinates), "X: %d, Y: %d", playerX, playerY);
        renderText(renderer, playerCoordinates, 1100, 10);
        renderScore(renderer, objectManager);

        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    destroyGameObjectManager(objectManager);
    SDL_DestroyTexture(moveSpriteSheet);
    SDL_DestroyTexture(idleSpriteSheet);
    SDL_DestroyTexture(map.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}