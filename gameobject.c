#include "gameobject.h"
#include "animation.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>

GameObjectManager* createGameObjectManager(SDL_Renderer* renderer, int count) {
    if (count != 3) {
        printf("Erreur : Le nombre d'objets doit être exactement 3\n");
        return NULL;
    }

    GameObjectManager* manager = malloc(sizeof(GameObjectManager));
    manager->objects = malloc(sizeof(GameObject) * count);
    manager->count = count;
    manager->collected = 0;
    manager->showVictoryMessage = false;

    // Chargement du son de victoire
    manager->victorySound = Mix_LoadWAV("win.mp3");
    if (!manager->victorySound) {
        printf("Erreur chargement son de victoire: %s\n", Mix_GetError());
    }

    // Position et taille du bouton de redémarrage
    manager->restartButton.x = 500;
    manager->restartButton.y = 500;
    manager->restartButton.w = 200;
    manager->restartButton.h = 50;

    // Positions prédéfinies pour les objets
    int positions[][2] = {
        {462, 1275},  // Objet 1
        {1437, 735},  // Objet 2
        {0, 0}   // Objet 3 (Caché)
    };

    for (int i = 0; i < count; i++) {
        char filename[20];
        snprintf(filename, sizeof(filename), "objet%d.bmp", i + 1);

        printf("Chargement de l'objet %d : %s\n", i + 1, filename);
        manager->objects[i].texture = loadTexture(filename, renderer);

        if (manager->objects[i].texture == NULL) {
            printf("ERREUR : Impossible de charger %s\n", filename);
            for (int j = 0; j < i; j++) {
                SDL_DestroyTexture(manager->objects[j].texture);
            }
            if (manager->victorySound) {
                Mix_FreeChunk(manager->victorySound);
            }
            free(manager->objects);
            free(manager);
            return NULL;
        }

        manager->objects[i].x = positions[i][0];
        manager->objects[i].y = positions[i][1];
        manager->objects[i].width = 32;
        manager->objects[i].height = 32;
        manager->objects[i].collected = false;
    }

    return manager;
}

void destroyGameObjectManager(GameObjectManager* manager) {
    if (manager) {
        if (manager->victorySound) {
            Mix_FreeChunk(manager->victorySound);
        }
        for (int i = 0; i < manager->count; i++) {
            SDL_DestroyTexture(manager->objects[i].texture);
        }
        free(manager->objects);
        free(manager);
    }
}

void renderGameObjects(SDL_Renderer* renderer, GameObjectManager* manager) {
    if (!manager) return;

    // Rendu des objets
    for (int i = 0; i < manager->count; i++) {
        if (!manager->objects[i].collected) {
            // Convertir les coordonnées absolues en coordonnées écran
            SDL_Rect destRect = {
                manager->objects[i].x - manager->viewPortX,
                manager->objects[i].y - manager->viewPortY,
                manager->objects[i].width,
                manager->objects[i].height
            };

            // Ne rendre que si l'objet est visible à l'écran
            if (destRect.x + destRect.w >= 0 && destRect.x <= 1280 &&
                destRect.y + destRect.h >= 0 && destRect.y <= 960) {
                SDL_RenderCopy(renderer, manager->objects[i].texture, NULL, &destRect);
            }
        }
    }

    // Afficher le message de victoire si tous les objets sont collectés
    if (manager->collected == manager->count) {
        manager->showVictoryMessage = true;

        // Créer une texture semi-transparente pour le fond
        SDL_Surface* surface = SDL_CreateRGBSurface(0, 400, 300, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 192));
        SDL_Texture* overlay = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureBlendMode(overlay, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surface);

        // Dessiner le fond semi-transparent
        SDL_Rect messageBox = { 350, 300, 551, 300 };
        SDL_RenderCopy(renderer, overlay, NULL, &messageBox);
        SDL_DestroyTexture(overlay);

        // Afficher le message
        TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
        if (font) {
            SDL_Color textColor = { 255, 255, 255, 255 };
            const char* message = "Bravo! Tu as récupéré les épées sacrées!";
            SDL_Surface* surface = TTF_RenderText_Solid(font, message, textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect textRect = { 420, 320, surface->w, surface->h };
                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }

            // Dessiner le bouton de redémarrage
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
            SDL_RenderFillRect(renderer, &manager->restartButton);

            // Texte du bouton
            SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Recommencer", textColor);
            if (buttonSurface) {
                SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
                SDL_Rect buttonTextRect = {
                    manager->restartButton.x + (manager->restartButton.w - buttonSurface->w) / 2,
                    manager->restartButton.y + (manager->restartButton.h - buttonSurface->h) / 2,
                    buttonSurface->w,
                    buttonSurface->h
                };
                SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonTextRect);
                SDL_DestroyTexture(buttonTexture);
                SDL_FreeSurface(buttonSurface);
            }

            TTF_CloseFont(font);
        }
    }
}
void checkCollisions(GameObjectManager* manager, int playerX, int playerY, int playerWidth, int playerHeight) {
    if (!manager) return;

    SDL_Rect playerRect = {
        playerX + manager->viewPortX,
        playerY + manager->viewPortY,
        playerWidth,
        playerHeight
    };

    for (int i = 0; i < manager->count; i++) {
        if (!manager->objects[i].collected) {
            SDL_Rect objectRect = {
                manager->objects[i].x,
                manager->objects[i].y,
                manager->objects[i].width,
                manager->objects[i].height
            };

            if (SDL_HasIntersection(&playerRect, &objectRect)) {
                manager->objects[i].collected = true;
                manager->collected++;
                printf("Objet %d collecté ! Score : %d/3\n", i + 1, manager->collected);

                // Jouer le son de victoire si tous les objets sont collectés
                if (manager->collected == manager->count && manager->victorySound) {
                    Mix_HaltMusic(); // Arrêter la musique
                    Mix_PlayChannel(-1, manager->victorySound, 0); // Jouer le son une fois
                    // Ne pas attendre la fin du son, laisser le jeu continuer
                }
            }
        }
    }
}

void renderScore(SDL_Renderer* renderer, GameObjectManager* manager) {
    if (!manager) return;

    char scoreText[20];
    snprintf(scoreText, sizeof(scoreText), "Score: %d/3", manager->collected);

    SDL_Color color = { 255, 255, 255, 255 };
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);

    if (font) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText, color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect dstRect = { 1100, 50, surface->w, surface->h };
                SDL_RenderCopy(renderer, texture, NULL, &dstRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
        TTF_CloseFont(font);
    }
}

void updateViewport(GameObjectManager* manager, int viewPortX, int viewPortY) {
    if (!manager) return;
    manager->viewPortX = viewPortX;
    manager->viewPortY = viewPortY;
}

bool handleRestartClick(GameObjectManager* manager, int mouseX, int mouseY) {
    if (!manager || !manager->showVictoryMessage) return false;

    SDL_Point point = { mouseX, mouseY };
    if (SDL_PointInRect(&point, &manager->restartButton)) {
        resetGame(manager);
        return true;
    }
    return false;
}

void resetGame(GameObjectManager* manager) {
    if (!manager) return;

    manager->collected = 0;
    manager->showVictoryMessage = false;

    for (int i = 0; i < manager->count; i++) {
        manager->objects[i].collected = false;
    }
}