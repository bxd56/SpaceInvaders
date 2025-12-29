# Space Invaders – C (ncurses / SDL)

Projet Space Invaders développé en langage C selon une architecture
**MVC (Modèle – Vue – Contrôleur)**.

Le jeu propose deux interfaces sélectionnables au lancement :
- une interface texte utilisant **ncurses**
- une interface graphique utilisant **SDL**

La logique du jeu est entièrement centralisée dans le modèle et est partagée
par les deux interfaces sans duplication de code.


## Dépendances

- GCC (C99 ou plus récent)
- ncurses (`libncurses-dev`)
- SDL3 et extensions : SDL3_image, SDL3_ttf 

## Compilation

Les instructions détaillées d’installation et de compilation sont disponibles
dans le fichier `INSTALL.txt`.

## Exécution

Après compilation, le binaire est généré dans le dossier `build/` sous le nom `spaceInvaders`.

Le choix de l’interface se fait via un argument en ligne de commande.

### Interface ncurses
```bash
./build/spaceInvaders ncurses
```

### Interface SDL
```bash
./build/spaceInvaders sdl
```
## Commandes clavier

### Déplacements et tirs
- ← (flèche gauche) : déplacer le vaisseau vers la gauche
- → (flèche droite) : déplacer le vaisseau vers la droite
- Espace : tirer

### Gestion de la partie
- `ESC` : quitter le jeu à tout moment
- `S` : démarrer la partie depuis le menu
- `R` : recommencer après un Game Over

## Architecture

Le projet suit une architecture MVC stricte :

### Modèle (`model.c / model.h`)
- Contient toute la logique du jeu :
  - gestion du joueur, ennemis, projectiles
  - collisions
  - niveaux, score, vies
  - explosions

### Vue
Deux vues interchangeables :
- `view_ncurses.c` : affichage texte via ncurses
- `view_sdl.c` : affichage graphique via SDL

Les vues :
- récupèrent l’état du modèle (positions, scores, vies, projectiles, etc.)
- dessinent le jeu à l’écran en fonction de cet état
- traduisent les entrées utilisateur en **commandes abstraites** (`CMD_LEFT`, `CMD_SHOOT`, etc.)
- ne contiennent aucune logique de jeu (aucune modification des positions, collisions, score, etc.)

### Contrôleur (`controller.c / controller.h`)
- Orchestre les interactions entre la vue et le modèle.
- Lit les commandes abstraites fournies par la vue.
- Met à jour le modèle en conséquence (déplacement du joueur, tirs, collisions).
- Gère la boucle principale du jeu :
    - Utilise une boucle avec framerate fixe (~60 FPS) grâce à un timestep contrôlé par `usleep`.
    - La logique du jeu (déplacements, tirs, collisions) est mise à jour à intervalles réguliers afin d’assurer un comportement identique entre ncurses et SDL.

## Licence 

Ce projet est sous licence MIT. Voir le fichier `LICENSE.txt` pour plus de détails