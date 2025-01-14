#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

typedef struct {
    int x, y;
    int width, height;
    bool isAttacking;
    int attackTimer;
    SDL_Texture* normalTexture;
    SDL_Texture* attackTexture;
    int health;
    Mix_Chunk* attackSound;
} Player;

void initPlayer(Player* player, SDL_Renderer* renderer);
void updatePlayer(Player* player);
void startAttack(Player* player);
void destroyPlayer(Player* player);

#endif