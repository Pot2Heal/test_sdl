#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include "map.h"
#include "animation.h"
#include "gameobject.h"
#include "player.h"
#include "enemy.h"
#include "menu.h"

typedef struct {
    int x, y, width, height;
} Rect;

// Zoom factor
const float zoomFactor = 1.6f;

// Zones interdites
Rect forbiddenZones[] = {
    {396, 268, 250, 220},  // Maison en haut � gauche    
    {872, 256, 400, 230},  // Maison en haut � droite
    {820, 573, 550, 250},  // Maison en bas � gauche
    {824, 1066, 500, 50},  // Barri�re en bas � gauche
    {398, 691, 275, 25},   // Barri�re en haut � droite
    {494, 1090, 220, 500}, // �tang
};

int isInForbiddenZone(int x, int y, Rect* zones, int numZones) {
    for (int i = 0; i < numZones; i++) {
        Rect zone = zones[i];
        if (x >= zone.x && x < zone.x + zone.width &&
            y >= zone.y && y < zone.y + zone.height) {
            return 1;
        }
    }
    return 0;
}

void renderText(SDL_Renderer* renderer, const char* text, int x, int y) {
    SDL_Color color = { 255, 255, 255, 255 };
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Erreur chargement de la police : %s\n", TTF_GetError());
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        printf("Erreur cr�ation surface de texte : %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Erreur cr�ation texture de texte : %s\n", TTF_GetError());
    }

    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}

void showWelcomeScreen(SDL_Renderer* renderer) {
    bool welcomeScreen = true;
    const char* message = "Bienvenue sur DragonBouleZ\n\nLe but est de r�cup�rer les 3 �p�es dispers�es\ndans le monde. Trouvez-les, que ce soit par terre\nou en tuant des ennemis.";

    while (welcomeScreen) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN) {
                welcomeScreen = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        TTF_Font* font = TTF_OpenFont("arial.ttf", 28);
        if (font) {
            SDL_Color textColor = { 0, 0, 0, 255 };
            int y = 300;
            char line[256];
            const char* start = message;
            const char* end = strchr(start, '\n');

            while (end) {
                int length = end - start;
                strncpy(line, start, length);
                line[length] = '\0';

                SDL_Surface* surface = TTF_RenderText_Solid(font, line, textColor);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect textRect = {
                        (1280 - surface->w) / 2,
                        y,
                        surface->w,
                        surface->h
                    };
                    SDL_RenderCopy(renderer, texture, NULL, &textRect);
                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(surface);
                }

                y += 40;
                start = end + 1;
                end = strchr(start, '\n');
            }

           
            SDL_Surface* surface = TTF_RenderText_Solid(font, start, textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect textRect = {
                    (1280 - surface->w) / 2,
                    y,
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }

            TTF_CloseFont(font);
        }

        // Ajouter un message pour continuer
        TTF_Font* smallFont = TTF_OpenFont("arial.ttf", 20);
        if (smallFont) {
            SDL_Color textColor = { 100, 100, 100, 255 };
            const char* continueMsg = "Cliquez ou appuyez sur une touche pour continuer...";
            SDL_Surface* surface = TTF_RenderText_Solid(smallFont, continueMsg, textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect textRect = {
                    (1280 - surface->w) / 2,
                    800,
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }
            TTF_CloseFont(smallFont);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    if (TTF_Init() < 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "DragonBouleZ - Le Retour",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 960, SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("Erreur cr�ation fen�tre: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur lors de l'initialisation de SDL_Mixer: %s\n", Mix_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur cr�ation renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    Mix_Music* musique = Mix_LoadMUS("musikaaa.mp3");
    if (!musique) {
        printf("Erreur lors du chargement de la musique: %s\n", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return -1;
    }

    Mix_PlayMusic(musique, -1);

    // Cr�ation du menu
    Menu* menu = createMenu(renderer);
    if (!menu) {
        printf("Erreur cr�ation menu\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    bool inMenu = true;
    bool running = true;

    while (running) {
        if (inMenu) {
            // Boucle du menu
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    break;
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    int menuAction = handleMenuClick(menu, mouseX, mouseY);

                    switch (menuAction) {
                    case MENU_PLAY:
                        inMenu = false;
                        showWelcomeScreen(renderer);
                        break;
                    case MENU_QUIT:
                        running = false;
                        break;
                    case MENU_RESOLUTION_CHANGE:
                        applyResolution(window, menu->resolutions[menu->currentResolution]);
                        break;
                    }
                }
            }

            // Rendu du menu
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            renderMenu(renderer, menu);
            SDL_RenderPresent(renderer);
        }
        else {
            // Initialisation du jeu
            GameObjectManager* objectManager = createGameObjectManager(renderer, 3);
            EnemyManager* enemyManager = createEnemyManager(renderer, 5, objectManager);
            Map map = loadMap("Startermap.bmp", renderer);

            int playerFrameWidth = 32, playerFrameHeight = 32;
            int playerNumFramesIdle = 4, playerNumFramesMove = 6;
            float playerScaleFactor = 3.0f;
            float playerSpeed = 200.0f;

            int playerX = 808;
            int playerY = 591;
            SDL_RendererFlip flip = SDL_FLIP_NONE;

            SDL_Texture* moveSpriteSheet = loadTexture("sprite.bmp", renderer);
            SDL_Texture* idleSpriteSheet = loadTexture("spriteidle.bmp", renderer);

            map.viewPort.w = (int)(1280 / zoomFactor);
            map.viewPort.h = (int)(960 / zoomFactor);

            // Initialiser le joueur
            Player player;
            player.x = playerX;
            player.y = playerY;
            player.width = playerFrameWidth;
            player.height = playerFrameHeight;
            player.health = 100;
            player.isAttacking = false;
            player.attackTimer = 0;
            initPlayer(&player, renderer);

            Uint32 lastTime = SDL_GetTicks();
            Uint32 frameDelay = 16;

            // Boucle principale du jeu
            bool inGame = true;
            while (inGame && running) {
                Uint32 frameStart = SDL_GetTicks();

                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false;
                        inGame = false;
                    }
                    else if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            inGame = false;
                            inMenu = true;
                        }
                    }
                    else if (event.type == SDL_MOUSEBUTTONDOWN) {
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            startAttack(&player);

                            for (int i = 0; i < enemyManager->count; i++) {
                                Enemy* enemy = &enemyManager->enemies[i];
                                if (!enemy->isDead) {
                                    int dx = (enemy->x - enemyManager->viewPortX) - (playerX - map.viewPort.x);
                                    int dy = (enemy->y - enemyManager->viewPortY) - (playerY - map.viewPort.y);
                                    float distance = sqrt(dx * dx + dy * dy);

                                    if (distance < 100) {
                                        damageEnemy(enemy, enemyManager);
                                    }
                                }
                            }

                            if (objectManager->showVictoryMessage) {
                                int mouseX = event.button.x;
                                int mouseY = event.button.y;
                                if (handleRestartClick(objectManager, mouseX, mouseY)) {
                                    playerX = 808;
                                    playerY = 591;
                                    destroyEnemyManager(enemyManager);
                                    enemyManager = createEnemyManager(renderer, 5, objectManager);
                                }
                            }
                        }
                    }
                }

                const Uint8* keyState = SDL_GetKeyboardState(NULL);
                int moving = 0;
                int newX = playerX, newY = playerY;

                if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) {
                    newY -= playerSpeed * (frameDelay / 1000.0f);
                    moving = 1;
                }
                if (keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN]) {
                    newY += playerSpeed * (frameDelay / 1000.0f);
                    moving = 1;
                }
                if (keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) {
                    newX -= playerSpeed * (frameDelay / 1000.0f);
                    moving = 1;
                    flip = SDL_FLIP_HORIZONTAL;
                }
                if (keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) {
                    newX += playerSpeed * (frameDelay / 1000.0f);
                    moving = 1;
                    flip = SDL_FLIP_NONE;
                }

                if (!isInForbiddenZone(newX, newY, forbiddenZones, sizeof(forbiddenZones) / sizeof(forbiddenZones[0]))) {
                    // Supprimer les limites arbitraires et ne garder que les forbidden zones
                    playerX = newX;
                    playerY = newY;
                }
                // Mise � jour du viewport pour suivre le joueur
                map.viewPort.x = playerX - (map.viewPort.w / 2);
                map.viewPort.y = playerY - (map.viewPort.h / 2);
                // Limites du viewport
                if (map.viewPort.x < 0) {
                    map.viewPort.x = 0;
                }
                if (map.viewPort.y < 0) {
                    map.viewPort.y = 0;
                }
                if (map.viewPort.x > map.width - map.viewPort.w) {
                    map.viewPort.x = map.width - map.viewPort.w;
                }
                if (map.viewPort.y > map.height - map.viewPort.h) {
                    map.viewPort.y = map.height - map.viewPort.h;
                }
                // Calcul de la position du joueur � l'�cran
                int screenX = playerX - map.viewPort.x;
                int screenY = playerY - map.viewPort.y;
                // Mise � jour des viewports
                updateViewport(objectManager, map.viewPort.x, map.viewPort.y);
                updateEnemyViewport(enemyManager, map.viewPort.x, map.viewPort.y);

                // Mise � jour des entit�s
                updateEnemies(enemyManager, playerX, playerY);
                updatePlayer(&player);

                // Rendu
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                // Rendu de la carte avec le viewport
                SDL_Rect destRect = { 0, 0, 1280, 960 };
                SDL_RenderCopy(renderer, map.texture, &map.viewPort, &destRect);

                renderGameObjects(renderer, objectManager);
                renderEnemies(renderer, enemyManager);

                // Rendu du joueur
                SDL_Texture* currentTexture = player.isAttacking ? player.attackTexture :
                    (moving ? moveSpriteSheet : idleSpriteSheet);
                int numFrames = player.isAttacking ? 4 : (moving ? playerNumFramesMove : playerNumFramesIdle);

                animateSprite(currentTexture, renderer, playerFrameWidth, playerFrameHeight,
                    numFrames, playerScaleFactor, screenX, screenY, flip);

                // V�rification des collisions
                checkCollisions(objectManager, screenX, screenY,
                    playerFrameWidth * playerScaleFactor,
                    playerFrameHeight * playerScaleFactor);

                // Affichage des coordonn�es et du score
                char playerCoordinates[50];
                snprintf(playerCoordinates, sizeof(playerCoordinates), "X: %d, Y: %d", playerX, playerY);
                renderText(renderer, playerCoordinates, 1100, 10);
                renderScore(renderer, objectManager);

                SDL_RenderPresent(renderer);

                // Gestion du framerate
                Uint32 frameTime = SDL_GetTicks() - frameStart;
                if (frameDelay > frameTime) {
                    SDL_Delay(frameDelay - frameTime);
                }
            }

            // Nettoyage des ressources du jeu
            destroyGameObjectManager(objectManager);
            destroyEnemyManager(enemyManager);
            SDL_DestroyTexture(moveSpriteSheet);
            SDL_DestroyTexture(idleSpriteSheet);
            SDL_DestroyTexture(map.texture);
        }
    }

    // Nettoyage final
    destroyMenu(menu);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}