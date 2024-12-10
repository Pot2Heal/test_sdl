#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    // Création de la Fenetre
    SDL_Window* window = SDL_CreateWindow("DragonBouleZ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {                                                                                                                                  
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    //Affichage de la Fenetre
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());                           
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    //importation de l'image de fond 
    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("mainmenu.bmp"));
    if (!background) {
        printf("Erreur du chargement de l'image: %s\n", SDL_GetError());              
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    //Boucle pour afficher l'image de Fond 
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {                                  
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        
        // Affichage de l'icone 
        SDL_Surface* icon = SDL_LoadBMP("icon.bmp");
        if (!icon) {
            printf("Erreur de chargement de l'icône: %s\n", SDL_GetError());
        }
        else {
            SDL_SetWindowIcon(window, icon);                   
            SDL_FreeSurface(icon);            
        }








        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);                                          // Liberation de l'espace 
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}