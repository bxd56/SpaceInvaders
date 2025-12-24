#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>

// --- CONSTANTES ---
#define MAX_ENEMIES 55
#define MAX_SHIELD 3
#define MAX_PLAYER_SHOTS 100
#define MAX_ENEMIES_SHOTS 100

// --- 1. DÉFINITIONS DE BASE ---
typedef enum {
    GAME_MENU,    // Le menu d'accueil
    GAME_PLAYING  // Le jeu en cours
} GameState;


typedef enum {
    DIR_UP,
    DIR_DOWN
} Direction;

typedef struct Projectile {
    int x, y;
    float speed;
    bool active;
    Direction dir;
} Projectile;

typedef struct Spaceship {
    int x, y, speedX, speedY;
    bool state; // Vivant ou mort
} Spaceship;

typedef struct Shield {
    int x, y;
    int resistance;
    bool active;
} Shield;

// --- 2. DÉFINITIONS DÉPENDANTES 
typedef struct Player {
    int lives, score;
    Spaceship spaceship; // Utilise struct Spaceship définie plus haut
} Player;

// --- 3. STRUCTURE PRINCIPALE ---

typedef struct GameArea {
    int width, height; // Attention à l'orthographe : height
    GameState state;
    Player player;
    Spaceship enemies[MAX_ENEMIES];
    Shield shields[MAX_SHIELD];
    Projectile playerShots[MAX_PLAYER_SHOTS];
    Projectile enemiesShots[MAX_ENEMIES_SHOTS];

    int playerProjec, enemiesProjec;

    // Variables d'état du jeu ajoutées récemment
    int enemiesDirection; 
    int currentLevel;     

    bool game_over;

} GameArea;


// --- PROTOTYPES DES FONCTIONS ---

void initGame(GameArea *game);
void manageCollisions(GameArea *game);
void manageEnemiesMovement(GameArea *game);
void manageLevels(GameArea *game);
void manageScoreLives(GameArea *game);


void playerShoot(GameArea *game);       // tir
void enemyShoot(GameArea *game);
void manageProjectiles(GameArea *game);

#endif