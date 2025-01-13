#include "menu.h"
#include <stdio.h>
#include <stdlib.h>

Menu* createMenu(SDL_Renderer* renderer) {
    Menu* menu = malloc(sizeof(Menu));
    if (!menu) return NULL;

    // Charger l'image de fond
    SDL_Surface* surface = SDL_LoadBMP("mainmenu.bmp");
    if (!surface) {
        printf("Erreur chargement mainmenu.bmp: %s\n", SDL_GetError());
        free(menu);
        return NULL;
    }
    menu->backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Charger le logo
    surface = SDL_LoadBMP("logo.bmp");
    if (!surface) {
        printf("Erreur chargement logo.bmp: %s\n", SDL_GetError());
        SDL_DestroyTexture(menu->backgroundTexture);
        free(menu);
        return NULL;
    }
    menu->logoTexture = SDL_CreateTextureFromSurface(renderer, surface);
    menu->logoRect = (SDL_Rect){
        .x = (1465 - surface->w) / 2,
        .y = 50,
        .w = surface->w,
        .h = surface->h
    };
    SDL_FreeSurface(surface);

    // Définir les boutons
    menu->playButton = (SDL_Rect){ 490, 400, 300, 60 };
    menu->optionsButton = (SDL_Rect){ 490, 500, 300, 60 };
    menu->quitButton = (SDL_Rect){ 490, 600, 300, 60 };
    menu->backButton = (SDL_Rect){ 490, 700, 300, 60 };

    // Initialiser les résolutions disponibles
    menu->resolutions = malloc(sizeof(Resolution) * 3);
    menu->resolutionButtons = malloc(sizeof(SDL_Rect) * 3);
    menu->resolutionCount = 3;

    menu->resolutions[0] = (Resolution){ 1280, 960 };
    menu->resolutions[1] = (Resolution){ 1920, 1080 };
    menu->resolutions[2] = (Resolution){ 2560, 1440 };

    for (int i = 0; i < menu->resolutionCount; i++) {
        menu->resolutionButtons[i] = (SDL_Rect){
            490,
            300 + (i * 70),
            300,
            60
        };
    }

    menu->currentResolution = 0;
    menu->showOptions = false;

    return menu;
}

void destroyMenu(Menu* menu) {
    if (menu) {
        SDL_DestroyTexture(menu->backgroundTexture);
        SDL_DestroyTexture(menu->logoTexture);
        free(menu->resolutions);
        free(menu->resolutionButtons);
        free(menu);
    }
}

void renderMenu(SDL_Renderer* renderer, Menu* menu) {
    // Rendre le fond
    SDL_RenderCopy(renderer, menu->backgroundTexture, NULL, NULL);

    // Rendre le logo
    SDL_RenderCopy(renderer, menu->logoTexture, NULL, &menu->logoRect);

    // Configurer la police
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    SDL_Color textColor = { 255, 255, 255, 255 };

    if (!menu->showOptions) {
        // Rendre les boutons du menu principal
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        SDL_RenderFillRect(renderer, &menu->playButton);
        SDL_RenderFillRect(renderer, &menu->optionsButton);
        SDL_RenderFillRect(renderer, &menu->quitButton);

        if (font) {
            // Texte des boutons
            SDL_Surface* surface = TTF_RenderText_Solid(font, "Jouer", textColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {
                menu->playButton.x + (menu->playButton.w - surface->w) / 2,
                menu->playButton.y + (menu->playButton.h - surface->h) / 2,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);

            surface = TTF_RenderText_Solid(font, "Options", textColor);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            textRect = (SDL_Rect){
                menu->optionsButton.x + (menu->optionsButton.w - surface->w) / 2,
                menu->optionsButton.y + (menu->optionsButton.h - surface->h) / 2,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);

            surface = TTF_RenderText_Solid(font, "Quitter", textColor);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            textRect = (SDL_Rect){
                menu->quitButton.x + (menu->quitButton.w - surface->w) / 2,
                menu->quitButton.y + (menu->quitButton.h - surface->h) / 2,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
    }
    else {
        // Rendre les options de résolution
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        for (int i = 0; i < menu->resolutionCount; i++) {
            SDL_RenderFillRect(renderer, &menu->resolutionButtons[i]);
            if (font) {
                char resText[32];
                snprintf(resText, sizeof(resText), "%dx%d",
                    menu->resolutions[i].width,
                    menu->resolutions[i].height);
                SDL_Surface* surface = TTF_RenderText_Solid(font, resText, textColor);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect textRect = {
                    menu->resolutionButtons[i].x + (menu->resolutionButtons[i].w - surface->w) / 2,
                    menu->resolutionButtons[i].y + (menu->resolutionButtons[i].h - surface->h) / 2,
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }
        }

        // Bouton retour
        SDL_RenderFillRect(renderer, &menu->backButton);
        if (font) {
            SDL_Surface* surface = TTF_RenderText_Solid(font, "Retour", textColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {
                menu->backButton.x + (menu->backButton.w - surface->w) / 2,
                menu->backButton.y + (menu->backButton.h - surface->h) / 2,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
    }

    if (font) {
        TTF_CloseFont(font);
    }
}

int handleMenuClick(Menu* menu, int x, int y) {
    SDL_Point point = { x, y };

    if (!menu->showOptions) {
        if (SDL_PointInRect(&point, &menu->playButton)) {
            return MENU_PLAY;
        }
        if (SDL_PointInRect(&point, &menu->optionsButton)) {
            menu->showOptions = true;
            return MENU_OPTIONS;
        }
        if (SDL_PointInRect(&point, &menu->quitButton)) {
            return MENU_QUIT;
        }
    }
    else {
        if (SDL_PointInRect(&point, &menu->backButton)) {
            menu->showOptions = false;
            return MENU_BACK;
        }
        for (int i = 0; i < menu->resolutionCount; i++) {
            if (SDL_PointInRect(&point, &menu->resolutionButtons[i])) {
                menu->currentResolution = i;
                return MENU_RESOLUTION_CHANGE;
            }
        }
    }
    return MENU_NOTHING;
}

void applyResolution(SDL_Window* window, Resolution resolution) {
    SDL_SetWindowSize(window, resolution.width, resolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}