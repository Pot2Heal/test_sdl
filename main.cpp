#include <stdio.h>
#include <SDL2/SDL.h>
#include "map.h"
#include "animation.h"

int main(int argc, char* argv[]) {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // Cr�ation de la fen�tre
    SDL_Window* window = SDL_CreateWindow(
        "DragonBouleZ - Le Retour",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 960, SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("Erreur cr�ation fen�tre: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Cr�ation du renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur cr�ation renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Chargement de la carte
    Map map = loadMap("Startermap.bmp", renderer);
    if (!map.texture) {
        printf("Erreur chargement de la carte.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // V�rification des dimensions de la carte
    if (map.width <= 0 || map.height <= 0) {
        printf("Dimensions invalides de la carte.\n");
        SDL_DestroyTexture(map.texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialisation des variables joueur
    int playerFrameWidth = 32;
    int playerFrameHeight = 32;
    int playerNumFrames = 6;
    float playerScaleFactor = 3.0f;

    int playerX = 320 - 16;
    int playerY = 240 - 16;
    float playerSpeed = 100.0f; // Vitesse en pixels par seconde

    // Initialisation du sprite
    SDL_Texture* spriteSheet = NULL;
    bool isMoving = false; // Indicateur de mouvement

    // Boucle principale
    bool running = true;
    SDL_Event event;

    Uint32 lastTime = SDL_GetTicks(); // Temps de la derni�re mise � jour
    float moveDelay = 0.2f; // D�lai entre les mouvements en secondes
    float timeSinceLastMove = 0.0f; // Temps �coul� depuis le dernier mouvement

    while (running) {
        // Gestion des �v�nements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // R�cup�ration des touches appuy�es
        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        bool moving = keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_S] ||
            keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_D];

        // Mise � jour de l'�tat de mouvement
        if (moving != isMoving) {
            isMoving = moving;

            // Nettoyage de l'ancienne texture
            if (spriteSheet) SDL_DestroyTexture(spriteSheet);

            // Chargement de la nouvelle texture
            if (isMoving) {
                spriteSheet = loadTexture("sprite.bmp", renderer); // Sprites de d�placement
                playerNumFrames = 6;
            }
            else {
                spriteSheet = loadTexture("spriteidle.bmp", renderer); // Sprites idle
                playerNumFrames = 4;
            }

            if (!spriteSheet) {
                printf("Erreur lors du chargement de la texture.\n");
            }
        }

        // Calcul du temps �coul�
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; 
        lastTime = currentTime;

        // Mise � jour du temps �coul� depuis le dernier mouvement
        timeSinceLastMove += deltaTime;

        // Mise � jour des coordonn�es du joueur avec les limites de la carte
        if (timeSinceLastMove >= moveDelay) {
            if ((keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) && playerY - (playerSpeed * deltaTime) >= 0) {
                playerY -= playerSpeed * moveDelay; // D�placement vers le haut
            }
            if ((keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN]) && playerY + (playerSpeed * deltaTime) <= map.height - playerFrameHeight) {
                playerY += playerSpeed * moveDelay; // D�placement vers le bas
            }
            if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && playerX - (playerSpeed * deltaTime) >= 0) {
                playerX -= playerSpeed * moveDelay; // D�placement vers la gauche
            }
            if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) && playerX + (playerSpeed * deltaTime) <= map.width - playerFrameWidth) {
                playerX += playerSpeed * moveDelay; // D�placement vers la droite
            }

            // R�initialiser le temps �coul�
            timeSinceLastMove = 0.0f;
        }
        // Mettre � jour la vue de la carte en fonction du zoom
        map.viewPort.w = (int)(1280 / 2.0f); // Zoom � 2.0
        map.viewPort.h = (int)(960 / 2.0f);

        // Centrer la vue de la carte sur le joueur
        map.viewPort.x = playerX - map.viewPort.w / 2;
        map.viewPort.y = playerY - map.viewPort.h / 2;

        // Emp�cher la vue de sortir des limites de la carte
        if (map.viewPort.x < 0) map.viewPort.x = 0;
        if (map.viewPort.y < 0) map.viewPort.y = 0;
        if (map.viewPort.x > map.width - map.viewPort.w) map.viewPort.x = map.width - map.viewPort.w;
        if (map.viewPort.y > map.height - map.viewPort.h) map.viewPort.y = map.height - map.viewPort.h;

        // Rendu de la carte
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fond noir
        SDL_RenderClear(renderer);
        renderMap(renderer, &map);

        // Animation et rendu du joueur avec les nouvelles coordonn�es
        if (spriteSheet) {
            animateSprite(spriteSheet, renderer, playerFrameWidth, playerFrameHeight, playerNumFrames, playerScaleFactor, playerX, playerY);
        }

        // Pr�sentation du rendu
        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    if (spriteSheet) SDL_DestroyTexture(spriteSheet);
    SDL_DestroyTexture(map.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
