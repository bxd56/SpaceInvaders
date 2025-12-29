// view_sdl.c
#include "view_sdl.h"
#include "model.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TILE 16  // Taille d'une case (modifiable)
#define SCREEN_W 80
#define SCREEN_H 40


// --- Variables globales ---
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Texture *tex_player  = NULL;
static SDL_Texture *tex_enemy   = NULL;
static SDL_Texture *tex_shield  = NULL;
static SDL_Texture *tex_player_shot = NULL;
static SDL_Texture *tex_enemy_shot  = NULL;
static SDL_Texture *tex_explosion = NULL;

static TTF_Font *font_small = NULL;
static TTF_Font *font_big   = NULL;


static SDL_FRect btn_play  = { SCREEN_W/2 - 100, SCREEN_H/2 - 50, 200, 50 };
static SDL_FRect btn_quit  = { SCREEN_W/2 - 100, SCREEN_H/2 + 20, 200, 50 };


// --- Fonctions internes ---
static SDL_Texture* load_texture(const char *file) {
    SDL_Surface *surf = IMG_Load(file);
    if (!surf){
        perror("Load texture image failed");
        return NULL;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    if (!tex) {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return NULL;
    }
    return tex;
}

// Initialisation SDL3
static void init_sdl() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        exit(1);
    }

    if (!TTF_Init()) {
        fprintf(stderr, "TTF_Init error: %s\n", SDL_GetError());
        exit(1);
    }
    // Crée la fenêtre
    window = SDL_CreateWindow(
        "Space Invaders",
        SCREEN_W * TILE,
        SCREEN_H * TILE,
        SDL_WINDOW_RESIZABLE // fenêtre redimensionnable
    );

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        exit(1);
    }

    // Récupère le renderer associé
    renderer = SDL_CreateRenderer(window,NULL);
    if (!renderer) {
        fprintf(stderr, "SDL_GetRenderer error: %s\n", SDL_GetError());
        exit(1);
    }

    // Charge les textures
    tex_player = load_texture("assets/player.png");
    tex_enemy  = load_texture("assets/enemy.png");
    tex_shield = load_texture("assets/shield.png");
    tex_player_shot = load_texture("assets/projectile.png");
    tex_enemy_shot  = load_texture("assets/projectile.png");
    tex_explosion = load_texture("assets/explosion.jpg");


    font_small = TTF_OpenFont("assets/font.ttf", 18);
    font_big   = TTF_OpenFont("assets/font.ttf", 36);

    if (!font_small || !font_big) {
        fprintf(stderr, "Erreur chargement font.ttf: %s\n", SDL_GetError());
        exit(1);
    }
}

// Fermeture SDL
static void close_sdl() {
    if (tex_player) SDL_DestroyTexture(tex_player);
    if (tex_enemy) SDL_DestroyTexture(tex_enemy);
    if (tex_shield) SDL_DestroyTexture(tex_shield);
    if (tex_player_shot) SDL_DestroyTexture(tex_player_shot);
    if (tex_enemy_shot) SDL_DestroyTexture(tex_enemy_shot);
    if (font_small) TTF_CloseFont(font_small);
    if (font_big)   TTF_CloseFont(font_big);
    
    if (window) SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

static Command input_sdl() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) return CMD_QUIT;

        if (e.type == SDL_EVENT_KEY_DOWN) {
            SDL_Scancode sc = e.key.scancode;
            switch (sc) {
                case SDL_SCANCODE_LEFT:  return CMD_LEFT;
                case SDL_SCANCODE_RIGHT: return CMD_RIGHT;
                case SDL_SCANCODE_SPACE: return CMD_SHOOT;
                case SDL_SCANCODE_S:     return CMD_START;
                case SDL_SCANCODE_R:     return CMD_RESTART;
                case SDL_SCANCODE_P:     return CMD_PAUSE;
                case SDL_SCANCODE_ESCAPE: return CMD_QUIT; 
                default: break;
            }
        }

        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = e.button.x;
            int my = e.button.y;
            if (mx >= btn_play.x && mx <= btn_play.x + btn_play.w &&
                my >= btn_play.y && my <= btn_play.y + btn_play.h) return CMD_START;
            if (mx >= btn_quit.x && mx <= btn_quit.x + btn_quit.w &&
                my >= btn_quit.y && my <= btn_quit.y + btn_quit.h) return CMD_QUIT;
        }
    }
    return CMD_NONE;
}
static void draw_text(const char *text, int x, int y, SDL_Color color, TTF_Font *font) {
    SDL_Surface *surf = TTF_RenderText_Blended(font, text,strlen(text),color);
    if (!surf){
        fprintf(stderr, "TTF_RenderText_Blended failed: %s\n", SDL_GetError());
        return;
    }

    int w = surf->w;
    int h = surf->h;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);

    SDL_FRect dst = { x, y, w, h };
    SDL_RenderTexture(renderer, tex, NULL, &dst);

    SDL_DestroyTexture(tex);
}

// Dessin du menu
static void draw_menu_sdl() {
    SDL_Color white = {255, 255, 255, 255};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_text("BIENVENUE A SPACE INVADERS", 160, 150, white, font_big);
    draw_text("APPUYEZ SUR S POUR COMMENCER", 180, 240, white, font_small);
    draw_text("APPUYEZ SUR ESC POUR QUITTER",   200, 280, white, font_small);

    SDL_RenderPresent(renderer);
}
static void draw_text_center(const char *text, int y, SDL_Color color, TTF_Font *font) {
    int w,h;
    if (!TTF_GetStringSize(font, text, strlen(text), &w, &h)) {
        fprintf(stderr, "Erreur TTF_GetStringSize: %s\n", SDL_GetError());
    }
    int x = (SCREEN_W * TILE - w)/2;
    draw_text(text, x, y, color, font);
}


// Dessin du jeu
static void draw_sdl(GameArea *game) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (game->state == GAME_MENU) {
        draw_menu_sdl();
        return;
    }

    if (game->state == GAME_PLAYING) {
        SDL_Color white = {255, 255, 255, 255};

        char hud[64];
        snprintf(hud, sizeof(hud),
                "SCORE: %d   VIES: %d   NIVEAU: %d",
                game->player.score,
                game->player.lives,
                game->currentLevel);

        draw_text(hud, 10, 10, white, font_small);
        // --- Joueur ---
        if (game->player.spaceship.state) {
            SDL_FRect dst = { game->player.spaceship.x * TILE,
                              game->player.spaceship.y * TILE,
                              TILE, TILE };
            SDL_RenderTexture(renderer, tex_player, NULL, &dst);
            if (!tex_player) fprintf(stderr, "Erreur chargement player.png\n");

        }

        // --- Ennemis ---
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (game->enemies[i].state) {
                SDL_FRect dst = { game->enemies[i].x * TILE,
                                  game->enemies[i].y * TILE,
                                  TILE, TILE };
                SDL_RenderTexture(renderer, tex_enemy, NULL, &dst);
                if (!tex_enemy) fprintf(stderr,"Erreur chargement enemy.png");
            }
        }

        // --- Boucliers ---
        for (int i = 0; i < MAX_SHIELD; i++) {
            if (game->shields[i].active) {
                SDL_FRect dst = { game->shields[i].x * TILE,
                                  game->shields[i].y * TILE,
                                  TILE, TILE };
                SDL_RenderTexture(renderer, tex_shield, NULL, &dst);
                if (!tex_shield) fprintf(stderr,"Erreur chargement enemy.png");
            }
        }

        // --- Projectiles joueur ---
        for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
            if (game->playerShots[i].active) {
                SDL_FRect dst = {
                    game->playerShots[i].x * TILE,
                    game->playerShots[i].y * TILE,
                    TILE/2, TILE
                };
                SDL_RenderTexture(renderer, tex_player_shot, NULL, &dst);
                if (!tex_player_shot) fprintf(stderr,"Erreur chargement projectile.png");
            }
        }

        // --- Projectiles ennemis ---
        for (int i = 0; i < MAX_ENEMIES_SHOTS; i++) {
            if (game->enemiesShots[i].active) {
                SDL_FRect dst = {
                    game->enemiesShots[i].x * TILE,
                    game->enemiesShots[i].y * TILE,
                    TILE/2, TILE
                };
                SDL_RenderTexture(renderer, tex_enemy_shot, NULL, &dst);
                if (!tex_enemy_shot) fprintf(stderr,"Erreur chargement projectile.png");
            }
        }
        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (game->explosions[i].active) {

                SDL_FRect dst = {
                    game->explosions[i].x * TILE,
                    game->explosions[i].y * TILE,
                    TILE,
                    TILE
                };

                SDL_FRect src = {
                    game->explosions[i].frame * TILE,
                    0,
                    TILE,
                    TILE
                };

                SDL_RenderTexture(renderer, tex_explosion, NULL, &dst);
            }
        }

        
    }

    // --- Game Over ---
    if (game->game_over) {
        SDL_Color red = {255, 0, 0, 255};
        SDL_Color white = {255, 255, 255, 255};

        draw_text_center("GAME OVER", (SCREEN_H*TILE)/2 - 50, red, font_big);
        draw_text_center("R POUR REJOUER", (SCREEN_H*TILE)/2 + 10, white, font_small);
        draw_text_center("ESC POUR QUITTER", (SCREEN_H*TILE)/2 + 50, white, font_small);


    }

    SDL_RenderPresent(renderer);
}

// --- Interface publique ---
ViewInterface get_sdl_interface() {
    ViewInterface v = {
        .init = init_sdl,
        .close = close_sdl,
        .get_input = input_sdl,
        .draw = draw_sdl
    };
    return v;
}
