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

//TODO Gerer les collisions 
void manageCollisions(GameArea *game){
    
}
//TODO Gerer du mouvemnt en groupe
void manageEnemiesMovement(){}

//TODO Gestion des niveaux
void manageLevels(){}

//TODO Gestion des scores et vies et collisions 
void manageScoreLives(){}
