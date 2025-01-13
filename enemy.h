#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "gameobject.h"

typedef struct {
    int x, y;
    int width, height;
    int health;
    bool isDead;
    SDL_Texture* texture;
    float speed;
    int currentFrame;
    Uint32 lastFrameTime;
    bool isDropper;
} Enemy;

typedef struct {
    Enemy* enemies;
    int count;
    int viewPortX;
    int viewPortY;
    GameObjectManager* objectManager;  // Ajout d'une référence au GameObjectManager
} EnemyManager;

EnemyManager* createEnemyManager(SDL_Renderer* renderer, int count, GameObjectManager* objectManager);
void destroyEnemyManager(EnemyManager* manager);
void renderEnemies(SDL_Renderer* renderer, EnemyManager* manager);
void updateEnemies(EnemyManager* manager, int playerX, int playerY);
void updateEnemyViewport(EnemyManager* manager, int viewPortX, int viewPortY);
void damageEnemy(Enemy* enemy, EnemyManager* manager);

#endif