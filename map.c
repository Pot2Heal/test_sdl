#include "map.h"
#include <stdio.h>
#include <stdlib.h>

// Charger la carte
Map loadMap(const char* filename, SDL_Renderer* renderer) {
    Map map = { 0 };

    // Charger la texture
    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        printf("Erreur : Impossible de charger la carte %s. SDL_Error : %s\n", filename, SDL_GetError());
        return map;
    }

    map.texture = SDL_CreateTextureFromSurface(renderer, surface);
    map.width = surface->w;
    map.height = surface->h;

    SDL_FreeSurface(surface);

    if (!map.texture) {
        printf("Erreur : Impossible de convertir la surface en texture. SDL_Error : %s\n", SDL_GetError());
        return map;
    }

    // Initialiser le viewport
    map.viewPort.x = 0;
    map.viewPort.y = 0;
    map.viewPort.w = (int)(800 / 1.6f);  // Conversion explicite pour éviter la perte de données
    map.viewPort.h = (int)(600 / 1.6f);  // Conversion explicite pour éviter la perte de données

    return map;
}

// Rendre la carte
void renderMap(SDL_Renderer* renderer, Map* map) {
    if (!map || !map->texture) return;

    SDL_RenderCopy(renderer, map->texture, &map->viewPort, NULL);
}

// Fonction pour déplacer la carte (scrolling)
void moveMap(Map* map, int deltaX, int deltaY) {
    if (!map) return;

    // Mettre à jour les coordonnées du viewport
    map->viewPort.x += deltaX;
    map->viewPort.y += deltaY;

    // S'assurer que le viewport ne sort pas des limites de la carte
    if (map->viewPort.x < 0) map->viewPort.x = 0;
    if (map->viewPort.y < 0) map->viewPort.y = 0;
    if (map->viewPort.x + map->viewPort.w > map->width)
        map->viewPort.x = map->width - map->viewPort.w;
    if (map->viewPort.y + map->viewPort.h > map->height)
        map->viewPort.y = map->height - map->viewPort.h;
}
