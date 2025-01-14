#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
    int width;
    int height;
} Resolution;

typedef struct {
    SDL_Texture* backgroundTexture;
    SDL_Texture* logoTexture;
    SDL_Rect logoRect;
    SDL_Rect playButton;
    SDL_Rect optionsButton;
    SDL_Rect quitButton;
    SDL_Rect backButton;
    SDL_Rect volumeSlider;
    int currentVolume;
    bool showOptions;
    Resolution* resolutions;
    int currentResolution;
    int resolutionCount;
    SDL_Rect* resolutionButtons;
} Menu;

Menu* createMenu(SDL_Renderer* renderer);
void destroyMenu(Menu* menu);
void renderMenu(SDL_Renderer* renderer, Menu* menu);
int handleMenuClick(Menu* menu, int x, int y);
void applyResolution(SDL_Window* window, Resolution resolution);
void updateVolume(Menu* menu, int x);

// Constantes pour les retours de handleMenuClick
#define MENU_NOTHING 0
#define MENU_PLAY 1
#define MENU_OPTIONS 2
#define MENU_QUIT 3
#define MENU_BACK 4
#define MENU_RESOLUTION_CHANGE 5
#define MENU_VOLUME_CHANGE 6

#endif