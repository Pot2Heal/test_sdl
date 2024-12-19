#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* textTexture[4];  // Tableau pour stocker les textures de texte
    SDL_Rect textRect[4];  // Tableau pour stocker les rectangles de position pour chaque texte

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 2;
    }

    // Création de la fenêtre
    window = SDL_CreateWindow(
        "Ma fenêtre avec plusieurs textes",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL
    );

    // Vérification si la fenêtre a bien été créée
    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 3;
    }

    // Création du renderer associé à la fenêtre
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 4;
    }

    
    font = TTF_OpenFont("./polices/The Wild Breath of Zelda.ttf", 28);  // Charger la police avec le chemin passé en argument
    if (font == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 5;
    }

    // Tableau des textes à afficher
    const char* texts[4] = { "Bienvenue dans dragonbouleZ", "Creation BTS CIEL", "Realiser par ANGOT HAROYAN ISRAEL DUFOUR", " INSHALLA SA MARCHE" };
    SDL_Color textColor[4] =
    {
        {152, 245, 249, 255},
        {210, 1, 3, 255},
        {125, 218, 88, 255},
        {6, 2, 112, 255}
    };

    // Création des textes et leurs textures
    for (int i = 0; i < 4; i++) {
        // Créer une surface pour chaque texte
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, texts[i], textColor[i]);
        if (textSurface == NULL) {
            printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
            TTF_CloseFont(font);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return 6;
        }

        // Créer une texture à partir de la surface
        textTexture[i] = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture[i] == NULL) {
            printf("Unable to create texture from text surface! SDL_Error: %s\n", SDL_GetError());
            SDL_FreeSurface(textSurface);
            TTF_CloseFont(font);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return 7;
        }

        // Définir la position et la taille de chaque texte
        textRect[i].x = 100;  // Position horizontale
        textRect[i].y = 100 + i * 100;  // Espacement vertical entre les textes
        textRect[i].w = textSurface->w;  // Largeur du texte
        textRect[i].h = textSurface->h;  // Hauteur du texte

        
        SDL_FreeSurface(textSurface);
    }

    // Définir la couleur de fond
    SDL_SetRenderDrawColor(renderer, 255, 255, 189, 255);
    SDL_RenderClear(renderer);  // Remplir l'écran avec la couleur définie

    // Afficher chaque texte avec un intervalle de 3 secondes
    for (int i = 0; i < 4; i++) {
        SDL_RenderClear(renderer);  // Efface
        SDL_RenderCopy(renderer, textTexture[i], NULL, &textRect[i]);  
        SDL_RenderPresent(renderer);  // Afficher les modifications à l'écran
        SDL_Delay(3000);  // Attendre 3 secondes avant d'afficher le texte suivant
    }

    // Libération des ressources
    for (int i = 0; i < 4; i++) {
        SDL_DestroyTexture(textTexture[i]);
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
