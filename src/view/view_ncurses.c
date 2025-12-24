#define _DEFAULT_SOURCE // Pour éviter certains warnings
#include "view_ncurses.h"
#include "model.h"
#include <ncurses.h>
#include <stdlib.h>

// --- FONCTIONS INTERNES (STATIC) ---

// Initialisation de Ncurses
static void init_nc() {
    initscr();              
    cbreak();               
    noecho();               
    keypad(stdscr, TRUE);   
    nodelay(stdscr, TRUE);  
    curs_set(0);            

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Joueur
        init_pair(2, COLOR_RED, COLOR_BLACK);    // Ennemi
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Projectile
        init_pair(4, COLOR_CYAN, COLOR_BLACK);   // Bouclier
    }
}

// Fermeture propre
static void close_nc() {
    endwin(); 
}

// Lecture des entrées
static Command input_nc() {
    int ch = getch();
    if (ch == ERR) return CMD_NONE;

    switch(ch) {
        case 'r':       return CMD_RESTART; // <--- Touche 'r'
        case 's':       return CMD_START;
        case KEY_LEFT:  return CMD_LEFT;
        case KEY_RIGHT: return CMD_RIGHT;
        case ' ':       return CMD_SHOOT;
        case 'p':       return CMD_PAUSE;
        case 'q':       return CMD_QUIT;
        default:        return CMD_NONE;
    }
}

// Helper pour dessiner un caractère coloré
static void draw_entity(int x, int y, char sprite, int color_pair) {
    attron(COLOR_PAIR(color_pair)); 
    mvaddch(y, x, sprite);          
    attroff(COLOR_PAIR(color_pair)); 
}

// Dessin du Menu d'accueil
static void draw_menu(GameArea *game) {
    int cx = game->width / 2;
    int cy = game->height / 2;

    mvprintw(cy - 2, cx - 13, "BIENVENUE A SPACE INVADERS");
    mvprintw(cy + 2, cx - 15, "Appuyez sur 's' pour commencer");
    mvprintw(cy + 3, cx - 15, "Appuyez sur 'q' pour quitter");
}

// Fonction principale d'affichage
static void draw_nc(GameArea *game) {
    erase(); // Efface l'écran

    // Cadre global
    box(stdscr, 0, 0); 

    if (game->state == GAME_MENU) {
        draw_menu(game);
    }
    else if (game->state == GAME_PLAYING) {
        // Infos
        mvprintw(0, 2, " Score: %d | Vies: %d | Niveau: %d ", 
                 game->player.score, game->player.lives, game->currentLevel);

        // Joueur
        if (game->player.spaceship.state) {
            draw_entity(game->player.spaceship.x, game->player.spaceship.y, 'A', 1);
        }

        // Ennemis
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (game->enemies[i].state) {
                draw_entity(game->enemies[i].x, game->enemies[i].y, 'M', 2);
            }
        }

        // Projectiles Joueur
        for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
             // On utilise .active maintenant
             if (game->playerShots[i].active) { 
                 draw_entity(game->playerShots[i].x, game->playerShots[i].y, '|', 3);
             }
        }

        for (int i = 0; i < MAX_ENEMIES_SHOTS; i++) {
             if (game->enemiesShots[i].active) {
                 // On dessine un 'v' ou '|' pour le tir ennemi
                 // On utilise la couleur 2 (celle des ennemis, souvent rouge)
                 draw_entity(game->enemiesShots[i].x, game->enemiesShots[i].y, 'v', 2);
             }
        }

        // Boucliers
        for (int i = 0; i < MAX_SHIELD; i++) {
            if (game->shields[i].active) {
                attron(COLOR_PAIR(4)); // Couleur Cyan
                
                // Affichage dynamique selon la résistance restante
                if (game->shields[i].resistance == 3) {
                    mvprintw(game->shields[i].y, game->shields[i].x, "===");
                } 
                else if (game->shields[i].resistance == 2) {
                    // On le décale un peu pour faire "centré" ou "abimé"
                    mvprintw(game->shields[i].y, game->shields[i].x, " ==");
                } 
                else if (game->shields[i].resistance == 1) {
                    mvprintw(game->shields[i].y, game->shields[i].x, "  =");
                }

                attroff(COLOR_PAIR(4));
            }
        }
    }
    
   if (game->game_over) {
        attron(COLOR_PAIR(2)); // Rouge pour le danger
        mvprintw(game->height / 2, (game->width / 2) - 5, "GAME OVER");
        attroff(COLOR_PAIR(2));
        
        mvprintw((game->height / 2) + 2, (game->width / 2) - 12, "Appuyez sur 'r' pour Rejouer");
        mvprintw((game->height / 2) + 3, (game->width / 2) - 12, "Appuyez sur 'q' pour Quitter");
    }

    refresh(); // Affiche tout ça
}

// --- INTERFACE PUBLIQUE ---

ViewInterface get_ncurses_interface() {
    ViewInterface v = {
        .init = init_nc,
        .close = close_nc,
        .get_input = input_nc,
        .draw = draw_nc
    };
    return v;
}