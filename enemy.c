#include "enemy.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    int x, y, width, height;
} Rect;

extern Rect forbiddenZones[];

bool isEnemyInForbiddenZone(int x, int y) {
    for (int i = 0; i < 6; i++) {
        Rect zone = forbiddenZones[i];
        if (x >= zone.x && x < zone.x + zone.width &&
            y >= zone.y && y < zone.y + zone.height) {
            return true;
        }
    }
    return false;
}

EnemyManager* createEnemyManager(SDL_Renderer* renderer, int count, GameObjectManager* objectManager) {
    srand(time(NULL));

    EnemyManager* manager = malloc(sizeof(EnemyManager));
    manager->enemies = malloc(sizeof(Enemy) * count);
    manager->count = count;
    manager->objectManager = objectManager;

    SDL_Surface* surface = SDL_LoadBMP("mechant.bmp");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Choisir aléatoirement un monstre qui fera tomber l'objet
    int dropperIndex = rand() % count;
    printf("Le monstre %d fera tomber l'objet 3\n", dropperIndex + 1);

    int fixedPositions[5][2] = {
        {240, 611},
        {561, 168},
        {833, 1088},
        {127, 915},
        {1416, 199}
    };

    for (int i = 0; i < count; i++) {
        manager->enemies[i].x = fixedPositions[i][0];
        manager->enemies[i].y = fixedPositions[i][1];
        manager->enemies[i].width = 32;
        manager->enemies[i].height = 32;
        manager->enemies[i].health = 100;
        manager->enemies[i].isDead = false;
        manager->enemies[i].texture = texture;
        manager->enemies[i].speed = 1.0f;
        manager->enemies[i].currentFrame = 0;
        manager->enemies[i].lastFrameTime = SDL_GetTicks();
        manager->enemies[i].isDropper = (i == dropperIndex);
    }

    return manager;
}

void destroyEnemyManager(EnemyManager* manager) {
    if (manager) {
        if (manager->enemies) {
            SDL_DestroyTexture(manager->enemies[0].texture);
            free(manager->enemies);
        }
        free(manager);
    }
}

void renderEnemies(SDL_Renderer* renderer, EnemyManager* manager) {
    const int FRAME_WIDTH = 32;
    const int FRAME_HEIGHT = 32;
    const int TOTAL_FRAMES = 4;
    const int FRAME_DELAY = 100;

    for (int i = 0; i < manager->count; i++) {
        Enemy* enemy = &manager->enemies[i];
        if (!enemy->isDead) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime > enemy->lastFrameTime + FRAME_DELAY) {
                enemy->currentFrame = (enemy->currentFrame + 1) % TOTAL_FRAMES;
                enemy->lastFrameTime = currentTime;
            }

            SDL_Rect srcRect = {
                enemy->currentFrame * FRAME_WIDTH,
                0,
                FRAME_WIDTH,
                FRAME_HEIGHT
            };

            // Convertir les coordonnées absolues en coordonnées écran
            SDL_Rect destRect = {
                enemy->x - manager->viewPortX,
                enemy->y - manager->viewPortY,
                enemy->width * 2,
                enemy->height * 2
            };

            // Ne rendre que si l'ennemi est visible à l'écran
            if (destRect.x + destRect.w >= 0 && destRect.x <= 1280 &&
                destRect.y + destRect.h >= 0 && destRect.y <= 960) {
                SDL_RenderCopy(renderer, enemy->texture, &srcRect, &destRect);
            }
        }
    }
}

void updateEnemies(EnemyManager* manager, int playerX, int playerY) {
    for (int i = 0; i < manager->count; i++) {
        Enemy* enemy = &manager->enemies[i];
        if (!enemy->isDead) {
            // Utiliser les coordonnées absolues du joueur pour le calcul
            float dx = (playerX + manager->viewPortX) - enemy->x;
            float dy = (playerY + manager->viewPortY) - enemy->y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 300 && distance > 5) {
                float newX = enemy->x + (dx / distance) * enemy->speed;
                float newY = enemy->y + (dy / distance) * enemy->speed;

                if (!isEnemyInForbiddenZone(newX, newY)) {
                    enemy->x = newX;
                    enemy->y = newY;
                }
            }
        }
    }
}

void updateEnemyViewport(EnemyManager* manager, int viewPortX, int viewPortY) {
    manager->viewPortX = viewPortX;
    manager->viewPortY = viewPortY;
}

void damageEnemy(Enemy* enemy, EnemyManager* manager) {
    if (!enemy->isDead) {
        enemy->health -= 25;  // Dégâts fixes de 25
        if (enemy->health <= 0) {
            enemy->isDead = true;

            // Vérifier si c'est bien l'ennemi marqué comme dropper
            if (enemy->isDropper && manager->objectManager != NULL) {
                // Faire apparaître l'objet 3 à la position exacte de l'ennemi mort
                manager->objectManager->objects[2].x = enemy->x;
                manager->objectManager->objects[2].y = enemy->y;
                manager->objectManager->objects[2].collected = false;
                printf("L'objet 3 est apparu à la position x:%d, y:%d\n", enemy->x, enemy->y);
            }
        }
    }
}