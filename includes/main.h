#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>

#define MENU_HOME        0
#define MENU_FILESELECT  1
#define MENU_GAME        2

extern uint8_t saveIndex;

extern int levelWidth;
extern int levelHeight;
extern uint8_t * levelTileMap;
extern const uint8_t solid_tiles[];
extern int score;
extern int coins;
extern int timer;
extern int life;
extern int is_paused;

#endif