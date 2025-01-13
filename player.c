#include "player.h"

void initPlayer(Player* player, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP("sprite.bmp");
    player->normalTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = SDL_LoadBMP("spriteattack.bmp");
    player->attackTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    player->isAttacking = false;
    player->attackTimer = 0;
}

void updatePlayer(Player* player) {
    if (player->isAttacking) {
        player->attackTimer++;
        if (player->attackTimer >= 30) { // 30 frames d'animation
            player->isAttacking = false;
            player->attackTimer = 0;
        }
    }
}

void startAttack(Player* player) {
    if (!player->isAttacking) {
        player->isAttacking = true;
        player->attackTimer = 0;
    }
}