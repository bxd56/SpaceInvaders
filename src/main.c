#define _DEFAULT_SOURCE // Pour éviter certains warnings
#include "controller.h"
#include <stdlib.h> // Pour srand
#include <time.h>


int main(int argc, char **argv) {
    // Initialisation du générateur pseudo-aléatoire
    srand(time(NULL));

    // Lancer le jeu avec le choix du backend
    run_game(argc, argv);

    return 0;
}