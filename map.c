#include "map.h"
#include <stdio.h>
#include <stdlib.h>

Map loadMap(const char* filename, SDL_Renderer* renderer) {
    Map map = { 0 };

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

    map.viewPort.x = 0;
    map.viewPort.y = 0;
    map.viewPort.w = (int)(800 / 1.6f);
    map.viewPort.h = (int)(600 / 1.6f);

    return map;
}

void renderMap(SDL_Renderer* renderer, Map* map) {
    if (!map || !map->texture) return;

    SDL_Rect destRect = { 0, 0, 1280, 960 };
    SDL_RenderCopy(renderer, map->texture, &map->viewPort, &destRect);
}

void moveMap(Map* map, int deltaX, int deltaY) {
    if (!map) return;

    map->viewPort.x += deltaX;
    map->viewPort.y += deltaY;

    if (map->viewPort.x < 0) map->viewPort.x = 0;
    if (map->viewPort.y < 0) map->viewPort.y = 0;
    if (map->viewPort.x > map->width - map->viewPort.w)
        map->viewPort.x = map->width - map->viewPort.w;
    if (map->viewPort.y > map->height - map->viewPort.h)
        map->viewPort.y = map->height - map->viewPort.h;
}