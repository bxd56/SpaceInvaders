/* Regroupe la structure */


#define MAX_ENEMIES 55
#define MAX_SHIELD 3
#define MAX_PLAYER_SHOTS 100
#define MAX_ENEMIES_SHOTS 100

typedef enum {DIR_UP,DIR_DOWN} Direction;

typedef struct Projectile {

    int x,y;

    float speed;
    //Les directions du tir pour la représentation en vue
    Direction dir;

} Projectile;


/*Structure pour le vaisseau (commun pour le joueur et les ennemi)*/
typedef struct Spaceship {

    //La position et la vitesse du vaisseau
    int x,y,speedX,speedY;
    //Un boolean pour savoir si le vaisseau est toujours "vivant"
    int state;

    
} Spaceship;

/* Structure pour le joueur pour les spécificités du joueur*/
typedef struct Player {

    int lives, score;
    Spaceship spaceship;

} Player;

/* Structure qui représente le bouclier */
typedef struct Shield {

    int x,y;
    int resistance;

} Shield;

typedef struct GameArea {

    int width,heigth;

    Player player;
    Spaceship enemies[MAX_ENEMIES];
    Shield shields[MAX_SHIELD];
    Projectile playerShots[MAX_PLAYER_SHOTS];
    Projectile enemiesShots[MAX_ENEMIES_SHOTS];

    int playerProjec,enemiesProjec;

} GameArea;



//TODO Initialisation
void initGame();

//TODO Gerer les collisions 
void manageCollisions();
//TODO Gerer du mouvemnt en groupe
void manageEnemiesMovement();

//TODO Gestion des niveaux
void manageLevels();

//TODO Gestion des scores et vies et collisions 
void manageScoreLives();