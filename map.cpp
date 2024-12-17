#include "map.h"
#include <SDL2/SDL.h>
#include <stdio.h>

// Charger la carte à partir d'un fichier
Map loadMap(const char* filename, SDL_Renderer* renderer) {
    Map map = { NULL, 0, 0, {0, 0, 1080, 480} }; // Vue initiale de la carte
    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        printf("Erreur lors du chargement de l'image de la carte: %s\n", SDL_GetError());
        return map;
    }

    map.texture = SDL_CreateTextureFromSurface(renderer, surface);
    map.width = surface->w;
    map.height = surface->h;
    SDL_FreeSurface(surface);

    return map;
}

// Afficher la carte
void renderMap(SDL_Renderer* renderer, Map* map) {
    SDL_RenderCopy(renderer, map->texture, &map->viewPort, NULL); // Afficher la zone visible de la carte
}
