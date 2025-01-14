#include "player.h"

void initPlayer(Player* player, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP("sprite.bmp");
    player->normalTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = SDL_LoadBMP("spriteattack.bmp");
    player->attackTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Chargement du son d'attaque
    player->attackSound = Mix_LoadWAV("claque.mp3");
    if (!player->attackSound) {
        printf("Erreur chargement son d'attaque: %s\n", Mix_GetError());
    }

    player->isAttacking = false;
    player->attackTimer = 0;
}

void updatePlayer(Player* player) {
    if (player->isAttacking) {
        player->attackTimer++;
        if (player->attackTimer >= 30) {
            player->isAttacking = false;
            player->attackTimer = 0;
        }
    }
}

void startAttack(Player* player) {
    if (!player->isAttacking) {
        player->isAttacking = true;
        player->attackTimer = 0;
        // Jouer le son d'attaque
        if (player->attackSound) {
            Mix_PlayChannel(-1, player->attackSound, 0);
        }
    }
}

void destroyPlayer(Player* player) {
    if (player->attackSound) {
        Mix_FreeChunk(player->attackSound);
    }
}