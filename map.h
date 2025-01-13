#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture* texture;
    int width, height;
    SDL_Rect viewPort;
} Map;

Map loadMap(const char* filename, SDL_Renderer* renderer);
void renderMap(SDL_Renderer* renderer, Map* map);
void moveMap(Map* map, int deltaX, int deltaY);

#endif