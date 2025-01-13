#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Texture* texture;
    int x, y;
    int width, height;
    bool collected;
} GameObject;

typedef struct {
    GameObject* objects;
    int count;
    int collected;
    int viewPortX;
    int viewPortY;
    bool showVictoryMessage;
    SDL_Rect restartButton;
} GameObjectManager;

GameObjectManager* createGameObjectManager(SDL_Renderer* renderer, int count);
void destroyGameObjectManager(GameObjectManager* manager);
void renderGameObjects(SDL_Renderer* renderer, GameObjectManager* manager);
void checkCollisions(GameObjectManager* manager, int playerX, int playerY, int playerWidth, int playerHeight);
void renderScore(SDL_Renderer* renderer, GameObjectManager* manager);
void updateViewport(GameObjectManager* manager, int viewPortX, int viewPortY);
bool handleRestartClick(GameObjectManager* manager, int mouseX, int mouseY);
void resetGame(GameObjectManager* manager);

#endif