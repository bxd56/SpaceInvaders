#include "model.h"
#include <stdio.h>
#include <stdlib.h>

//TODO Initialisation
void initGame(GameArea *game){

    Spaceship pl = {};
    pl.state = true;
    Player player = {.lives = 3 , .score = 0 , .spaceship = pl};
    game->player = player;
    game->width = 100;
    game->height = 100;

    /*enemies initalisation*/

    for(int i=0; i<MAX_ENEMIES;i++){
        Spaceship enemy = {};
        enemy.x = i*5;
        enemy.y = 10;
        enemy.speedX = 1;
        enemy.speedY = 0;
        enemy.state = true;
        game->enemies[i] = enemy;
    }

    /*shields initialisation*/
    for(int i=0; i<MAX_SHIELD;i++){
        Shield shield = {};
        shield.x = i*15;
        shield.y = 80;
        shield.resistance = 3;
        shield.active = true;
        game->shields[i] = shield;
    }

    game->game_over = false;



}
void manageCollisions(GameArea *game){
    for (int i = 0; i < MAX_PLAYER_SHOTS; i++){
        Projectile *shot = &game->playerShots[i];

        if (shot->y < 0) continue;

        for (int e = 0; e < MAX_ENEMIES; e++){
            Spaceship *enemy = &game->enemies[e];
            if (!enemy->state) continue;

            if (shot->x >= enemy->x && shot->x <= enemy->x + 20 &&
                shot->y >= enemy->y && shot->y <= enemy->y + 20) {
                    
                    enemy->state = 0;
                    shot->y = -10;

                    game->player.score += 100;
                }
        }
    }
    for (int i = 0; i < MAX_ENEMIES_SHOTS ; i++){
        Projectile *shot = &game->enemiesShots[i];
        if (shot->y < 0) continue;

        Spaceship *p = &game->player.spaceship;

        if (shot->x >= p->x && shot->x <= p->x + 20 &&
            shot->y >= p->y && shot->y <= p->y + 20) {

            game->player.lives--;
            shot->y = -10;
        }
    }
    for (int s = 0; s < MAX_SHIELD; s++) {

        Shield *shield = &game->shields[s];

        if (shield->resistance <= 0) continue;

        // tirs du joueur
        for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
            Projectile *shot = &game->playerShots[i];

            if (shot->x >= shield->x && shot->x <= shield->x + 40 &&
                shot->y >= shield->y && shot->y <= shield->y + 20) {

                shot->y = -10;
                shield->resistance--;
            }
        }
        for (int i = 0; i < MAX_ENEMIES_SHOTS; i++) {
            Projectile *shot = &game->enemiesShots[i];

            if (shot->x >= shield->x && shot->x <= shield->x + 40 &&
                shot->y >= shield->y && shot->y <= shield->y + 20) {

                shot->y = -10;
                shield->resistance--;
            }
        }
    }
}
void manageEnemiesMovement(GameArea *game){
    int needDown = 0;

    for (int i = 0; i < MAX_ENEMIES; i ++){
        if (!game->enemies[i].state) continue;

        if ((game->enemies[i].x >= game->width - 40 && enemiesDirection == 1) ||
            (game->enemies[i].x <= 10 && enemiesDirection == -1)) {
            needDown = 1;
            enemiesDirection = -enemiesDirection; // inverse le sens
            break;
        }
    }

    // Déplacer les ennemis
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].state == 0) continue;

        game->enemies[i].x += enemiesDirection * game->enemies[i].speedX;

        if (needDown) game->enemies[i].y += 20;
    }
}
void manageLevels(GameArea *game){
    int alive = 0;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].state == 1)
            alive = 1;
    }

    // Si plus personne → nouveau niveau
    if (!alive) {
        currentLevel++;
        initEnemies(game->enemies, MAX_ENEMIES);

        // augmenter un peu la vitesse pour difficulté
        for (int i = 0; i < MAX_ENEMIES; i++) {
            game->enemies[i].speedX += currentLevel; 
        }
    }
}
void manageScoreLives(GameArea *game){
    if (game->player.lives <= 0) {
        printf("GAME OVER ! Score final : %d\n", game->player.score);
        // ici tu peux mettre : exit(0);
    }

    // si le joueur a été touché, on le replace
    if (game->player.spaceship.y < 0) {
        game->player.spaceship.x = game->width / 2;
        game->player.spaceship.y = game->heigth - 50;
    }
}
