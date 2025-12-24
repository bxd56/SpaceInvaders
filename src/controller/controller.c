#define _DEFAULT_SOURCE
#include "controller.h"
#include "model.h"
#include "view_ncurses.h"
#include "view_sdl.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define FRAME_TIME_US 16666

void run_game(int argc, char **argv) {
    GameArea game;
    initGame(&game);

    ViewInterface view;

    /* Choix de la vue APRÈS compilation */
    if (argc > 1 && strcmp(argv[1], "sdl") == 0) {
        view = get_sdl_interface();
    } else if (argc > 1 && strcmp(argv[1], "ncurses") == 0){
        view = get_ncurses_interface();
    } else {
        fprintf(stderr,"<USAGE> Exemple : ./build/spaceInvaders sdl");
        exit(EXIT_FAILURE);
    }

    view.init();

    bool running = true;
    int frame = 0;

    while (running) {
        clock_t start = clock();

        Command cmd = view.get_input();

        if (cmd == CMD_QUIT)
            running = false;

        if (game.game_over) {
            if (cmd == CMD_RESTART) {
                initGame(&game);
                game.state = GAME_PLAYING;
            }
        }
        else if (game.state == GAME_MENU) {
            if (cmd == CMD_START)
                game.state = GAME_PLAYING;
        }
        else if (game.state == GAME_PLAYING) {

            if (cmd == CMD_LEFT && game.player.spaceship.x > 0)
                game.player.spaceship.x -= game.player.spaceship.speedX;

            if (cmd == CMD_RIGHT && game.player.spaceship.x < game.width)
                game.player.spaceship.x += game.player.spaceship.speedX;

            if (cmd == CMD_SHOOT)
                playerShoot(&game);

            manageProjectiles(&game);

            if (frame % 40 == 0)
                manageEnemiesMovement(&game);

            if (rand() % 50 == 0)
                enemyShoot(&game);

            manageCollisions(&game);
            manageLevels(&game);
            manageScoreLives(&game);
        }

        view.draw(&game);

        clock_t end = clock();
        double elapsed_us = ((double)(end - start) / CLOCKS_PER_SEC) * 1000000;
        if (elapsed_us < FRAME_TIME_US)
            usleep(FRAME_TIME_US - elapsed_us);

        frame++;
    }

    view.close();
}
