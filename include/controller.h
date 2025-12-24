#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

typedef enum {
    CMD_NONE,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_SHOOT,
    CMD_PAUSE,
    CMD_QUIT,
    CMD_START,
    CMD_RESTART 
} Command;

typedef struct ViewInterface {
    void (*init)(void);
    void (*close)(void);
    Command (*get_input)(void);
    void (*draw)(GameArea *game);
} ViewInterface;

void run_game(int argc,char **argv);

#endif