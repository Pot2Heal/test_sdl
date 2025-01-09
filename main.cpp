#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "map.h"
#include "animation.h"

typedef struct {
    int x, y, width, height;
} Rect;

// Zoom factor
const float zoomFactor = 1.6f;

//Zones Interdite  (x, y, largeur, hauteur)
Rect forbiddenZones[] = {
    {399, 245, 175, 175}, //Maison en haut a Gauche
    {695, 259, 175, 165},// Maison en haut a Droite
    {655, 477, 225, 145}, // Maison en bas a Gauche
    {395, 537, 150, 25},// Barriere Millieu Gauche
    {663, 707, 185, 25},// Barrierre En bas a Droite
    {455, 712, 125, 250},// Etang d'eau
};

bool isInForbiddenZone(int x, int y, Rect* zones, int numZones) {
    for (int i = 0; i < numZones; i++) {
        Rect zone = zones[i];
        if (x >= zone.x && x < zone.x + zone.width && y >= zone.y && y < zone.y + zone.height) {
            return true; // Le joueur est dans une zone interdite
        }
    }
    return false; // Le joueur n'est pas dans une zone interdite
}

void renderText(SDL_Renderer* renderer, const char* text, int x, int y) {
    SDL_Color color = { 255, 255, 255, 255 }; // Blanc
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24); // Police de caractère
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
    // Initialisation de SDL et SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    if (TTF_Init() < 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow(
        "DragonBouleZ - Le Retour",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 960, SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Création du renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Chargement de la carte
    Map map = loadMap("Startermap.bmp", renderer);
    if (!map.texture) {
        printf("Erreur chargement de la carte: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialisation des variables joueur
    int playerFrameWidth = 32;  // Largeur d'un sprite
    int playerFrameHeight = 32; // Hauteur d'un sprite
    int playerNumFramesIdle = 4;
    int playerNumFramesMove = 6;
    float playerScaleFactor = 3.0f;

    SDL_RendererFlip flip = SDL_FLIP_NONE;

    // Positionner le joueur au milieu de la carte
    int playerX = map.width / 2 - playerFrameWidth / 2; // Position initiale X
    int playerY = map.height / 2 - playerFrameHeight / 2; // Position initiale Y
    float playerSpeed = 200.0f; // Vitesse en pixels par seconde

    SDL_Texture* moveSpriteSheet = loadTexture("sprite.bmp", renderer);
    SDL_Texture* idleSpriteSheet = loadTexture("spriteidle.bmp", renderer);
    SDL_Texture* currentSpriteSheet = idleSpriteSheet;

    if (!moveSpriteSheet || !idleSpriteSheet) {
        printf("Erreur lors du chargement des sprites: %s\n", SDL_GetError());
        SDL_DestroyTexture(map.texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialisation de la caméra
    map.viewPort.w = (int)(1280 / zoomFactor); // Zoom à 1.2x
    map.viewPort.h = (int)(960 / zoomFactor);

    bool running = true;
    SDL_Event event;

    Uint32 lastTime = SDL_GetTicks(); // Temps de la dernière mise à jour
    Uint32 frameDelay = 16; // Délai pour 60 FPS (environ 16ms)
    Uint32 frameStart, frameTime;

    while (running) {
        frameStart = SDL_GetTicks();

        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Récupération des touches appuyées
        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        bool moving = false;

        // Mise à jour des coordonnées du joueur
        int newX = playerX;
        int newY = playerY;

        if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) {
            newY -= playerSpeed * (frameDelay / 1000.0f); // Déplacement vers le haut
            moving = true;
        }
        if (keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN]) {
            newY += playerSpeed * (frameDelay / 1000.0f); // Déplacement vers le bas
            moving = true;
        }
        if (keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) {
            newX -= playerSpeed * (frameDelay / 1000.0f); // Déplacement vers la gauche
            moving = true;
            flip = SDL_FLIP_HORIZONTAL; // Inverser le sprite
        }
        if (keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) {
            newX += playerSpeed * (frameDelay / 1000.0f); // Déplacement vers la droite
            moving = true;
            flip = SDL_FLIP_NONE; // Pas de flip
        }

        // Vérifier si la nouvelle position du joueur est dans une zone interdite
        if (!isInForbiddenZone(newX, newY, forbiddenZones, sizeof(forbiddenZones) / sizeof(forbiddenZones[0]))) {
            playerX = newX; // Appliquer la nouvelle position si valide
            playerY = newY;
        }

        // Changer le sprite entre mouvement et idle
        currentSpriteSheet = moving ? moveSpriteSheet : idleSpriteSheet;

        // Empêcher le joueur de sortir des limites de la carte
        if (playerX < 0) playerX = 0;
        if (playerY < 0) playerY = 0;
        if (playerX > map.width - playerFrameWidth) playerX = map.width - playerFrameWidth;
        if (playerY > map.height - playerFrameHeight) playerY = map.height - playerFrameHeight;

        // Mise à jour de la vue de la carte pour suivre le joueur
        map.viewPort.x = playerX - map.viewPort.w / 2;
        map.viewPort.y = playerY - map.viewPort.h / 2;

        // Empêcher la vue de sortir des limites de la carte
        if (map.viewPort.x < 0) map.viewPort.x = 0;
        if (map.viewPort.y < 0) map.viewPort.y = 0;
        if (map.viewPort.x > map.width - map.viewPort.w) map.viewPort.x = map.width - map.viewPort.w;
        if (map.viewPort.y > map.height - map.viewPort.h) map.viewPort.y = map.height - map.viewPort.h;

        // Rendu de la carte
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fond noir
        SDL_RenderClear(renderer);
        renderMap(renderer, &map);

        // Animation du joueur
        if (moving) {
            animateSprite(moveSpriteSheet, renderer, playerFrameWidth, playerFrameHeight, playerNumFramesMove, playerScaleFactor, playerX, playerY, flip);
        }
        else {
            animateSprite(idleSpriteSheet, renderer, playerFrameWidth, playerFrameHeight, playerNumFramesIdle, playerScaleFactor, playerX, playerY, flip);
        }

        // Afficher les coordonnées du joueur en haut à droite de l'écran
        char playerCoordinates[50];
        sprintf_s(playerCoordinates, "X: %d, Y: %d", playerX, playerY);
        renderText(renderer, playerCoordinates, 1100, 10);

        // Présentation du rendu
        SDL_RenderPresent(renderer);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Nettoyage
    SDL_DestroyTexture(moveSpriteSheet);
    SDL_DestroyTexture(idleSpriteSheet);
    SDL_DestroyTexture(map.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
