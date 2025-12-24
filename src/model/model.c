#include "model.h"
#include <stdio.h>
#include <stdlib.h>

static void initEnemies(GameArea *game) {
    int cols = 11; // 5 * 11 = 55 ennemis

    for(int i=0; i < MAX_ENEMIES; i++){
        Spaceship enemy = {0};
        // Disposition en grille
        enemy.x = (i % cols) * 6 + 2; // Ecartement horizontal
        enemy.y = (i / cols) * 4 + 2; // Ecartement vertical
        
        enemy.speedX = 1 + (game->currentLevel * 0.5); // Vitesse augmente avec le niveau
        enemy.speedY = 0;
        enemy.state = true;
        game->enemies[i] = enemy;
    }
}

void initGame(GameArea *game){
    // Initialisation dimension 
    game->width = 80;
    game->height = 40; // Hauteur arbitraire pour le terminal

    game->game_over = false;
    game->state = GAME_MENU;
    
    // Initialisation niveau et direction des ennemis
    game->currentLevel = 1;
    game->enemiesDirection = 1; // 1 = Droite

    // Joueur
    Spaceship pl = {0};
    pl.x = game->width / 2;
    pl.y = game->height - 2;
    pl.speedX = 1;
    pl.state = true;
    
    Player player = {.lives = 3 , .score = 0 , .spaceship = pl};
    game->player = player;

    // Ennemis 
    initEnemies(game);

    /*shields initialisation*/
    for(int i=0; i<MAX_SHIELD;i++){
        Shield shield = {0};
        shield.x = 10 + (i * 30);
        shield.y = game->height - 10;
        shield.resistance = 3;
        shield.active = true;
        game->shields[i] = shield;
    }
}

void manageEnemiesMovement(GameArea *game){
    bool changeDir = false;

    for(int i=0; i<MAX_ENEMIES; i++) {
        if (!game->enemies[i].state) continue;

        // Déplacement
        game->enemies[i].x += (game->enemies[i].speedX * game->enemiesDirection);

        // Vérification des bords
        // CORRECTION : Utilisation de game->enemiesDirection
        if ((game->enemies[i].x >= game->width - 2 && game->enemiesDirection == 1) ||
            (game->enemies[i].x <= 1 && game->enemiesDirection == -1)) {
            changeDir = true;
        }
    }

    if (changeDir) {
        game->enemiesDirection *= -1; // Inverse la direction
        for(int i=0; i<MAX_ENEMIES; i++) {
            game->enemies[i].y += 1; // Descend d'une ligne
        }
    }
}

void manageLevels(GameArea *game){
    // Vérifier si tous les ennemis sont morts
    bool allDead = true;
    for(int i=0; i<MAX_ENEMIES; i++){
        if(game->enemies[i].state) {
            allDead = false;
            break;
        }
    }

    if(allDead){
        // CORRECTION : utilisation de game->currentLevel
        game->currentLevel++;
        // On remet les ennemis (plus rapides grâce à initEnemies qui utilise currentLevel)
        initEnemies(game);
    }
}



void manageScoreLives(GameArea *game){
    // CORRECTION : typo heigth -> height
    if (game->player.spaceship.y > game->height) {
        game->game_over = true;
    }
}


// ... (tes fonctions initGame et manageEnemiesMovement restent là) ...

// Fonction pour tirer
void playerShoot(GameArea *game) {
    // Parcourir le tableau de projectiles pour trouver une place libre
    for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
        if (!game->playerShots[i].active) { // Si cet emplacement est vide
            game->playerShots[i].x = game->player.spaceship.x; // Position du vaisseau
            game->playerShots[i].y = game->player.spaceship.y - 1; // Juste au-dessus
            game->playerShots[i].speed = 1; // Vitesse vers le haut
            game->playerShots[i].active = true; // Le tir est vivant !
            break; // On a tiré une balle, on arrête la boucle
        }
    }
}


// Fonction pour gérer TOUS les projectiles (joueur et ennemis)
void manageProjectiles(GameArea *game) {
    // 1. Tirs du Joueur (Monte)
    for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
        if (game->playerShots[i].active) {
            game->playerShots[i].y -= 1;
            if (game->playerShots[i].y < 0) game->playerShots[i].active = false;
        }
    }

    // 2. Tirs des Ennemis (Descend)
    static int enemy_shot_timer = 0; 
    
    enemy_shot_timer++; // On compte les frames

    // C'EST ICI QUE TU REGLES LA VITESSE
    // Si timer > 3 : Les balles bougent tous les 3 tours (Vitesse / 3)

    if (enemy_shot_timer > 3) { 
        
        for (int i = 0; i < MAX_ENEMIES_SHOTS; i++) {
            if (game->enemiesShots[i].active) {
                game->enemiesShots[i].y += 1; // Descend d'une case
                
                // Si touche le bas de l'écran
                if (game->enemiesShots[i].y >= game->height) {
                    game->enemiesShots[i].active = false;
                }
            }
        }
        
        enemy_shot_timer = 0; // On remet le minuteur à zéro
    }
}


// Fonction pour faire tirer un ennemi au hasard
void enemyShoot(GameArea *game) {
    // Trouver un ennemi vivant au hasard
    // On essaie 10 fois de trouver un tireur pour ne pas boucler à l'infini
    for(int k=0; k<10; k++) {
        int r = rand() % MAX_ENEMIES;
        if (game->enemies[r].state) { // Si cet ennemi est vivant
            
            // Trouver une place libre dans le tableau des tirs ennemis
            for (int i = 0; i < MAX_ENEMIES_SHOTS; i++) {
                if (!game->enemiesShots[i].active) {
                    game->enemiesShots[i].x = game->enemies[r].x;
                    game->enemiesShots[i].y = game->enemies[r].y + 1;
                    game->enemiesShots[i].active = true;
                    return; // Tir effectué, on s'arrête
                }
            }
            return;
        }
    }
}

// Gestion des Collisions
void manageCollisions(GameArea *game) {
    int i;
    // 1. Collision : Tir Joueur -> Ennemi 
    for (i = 0; i < MAX_PLAYER_SHOTS; i++) {
        if (!game->playerShots[i].active) continue;
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!game->enemies[j].state) continue;
            if (game->playerShots[i].x == game->enemies[j].x && 
                game->playerShots[i].y == game->enemies[j].y) {
                game->enemies[j].state = false;
                game->playerShots[i].active = false;
                game->player.score += 10;
            }
        }
    }

    // 2. Collision : Tir Ennemi -> Joueur 
    for (i = 0; i < MAX_ENEMIES_SHOTS; i++) {
        if (!game->enemiesShots[i].active) continue;
        if (game->enemiesShots[i].x == game->player.spaceship.x && 
            game->enemiesShots[i].y == game->player.spaceship.y) {
            game->enemiesShots[i].active = false;
            game->player.lives -= 1;
            if (game->player.lives <= 0) game->game_over = true;
        }
    }

    // ---  GESTION DES BOUCLIERS ---
    
    for (int s = 0; s < MAX_SHIELD; s++) {
        if (!game->shields[s].active) continue; // Si le bouclier est déjà détruit, on passe

        // A. Vérifier les tirs du JOUEUR sur le bouclier
        for (i = 0; i < MAX_PLAYER_SHOTS; i++) {
            if (!game->playerShots[i].active) continue;

            // Hitbox large : le bouclier fait 3 caractères (x, x+1, x+2)
            if (game->playerShots[i].y == game->shields[s].y && 
                game->playerShots[i].x >= game->shields[s].x && 
                game->playerShots[i].x <= game->shields[s].x + 2) {
                
                game->playerShots[i].active = false; // Le tir disparait
                game->shields[s].resistance--;       // Le bouclier s'abime
            }
        }

        // B. Vérifier les tirs des ENNEMIS sur le bouclier
        for (i = 0; i < MAX_ENEMIES_SHOTS; i++) {
            if (!game->enemiesShots[i].active) continue;

            // Hitbox large identique
            if (game->enemiesShots[i].y == game->shields[s].y && 
                game->enemiesShots[i].x >= game->shields[s].x && 
                game->enemiesShots[i].x <= game->shields[s].x + 2) {
                
                game->enemiesShots[i].active = false; // Le tir disparait
                game->shields[s].resistance--;        // Le bouclier s'abime
            }
        }

        // Si la résistance tombe à 0, le bouclier disparaît
        if (game->shields[s].resistance <= 0) {
            game->shields[s].active = false;
        }
    }

   // Collision ENNEMI -> JOUEUR
    for (i = 0; i < MAX_ENEMIES; i++) {
        // Si l'ennemi est déjà mort, on s'en fiche
        if (!game->enemies[i].state) continue;

        // On vérifie si l'ennemi est au même endroit que le joueur
        // On regarde si la position Y est la même ET si la position X est proche (hitbox de 1 ou 2 cases)
        if (game->enemies[i].y == game->player.spaceship.y && 
            abs(game->enemies[i].x - game->player.spaceship.x) < 2) {
            
            // C'est perdu 
            game->player.lives = 0; 
            game->game_over = true;
        }
    }


    // Si un ennemi touche le bas de l'écran
    if (game->enemies[i].y >= game->height - 1) {
        game->game_over = true;
    }
}