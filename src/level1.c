#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "marioSprite.h"
#include "selectFileMenu.h"
#include "mario.h"
#include "sprite.h"
#include "level1Map.h"
#include "level1Sprite.h"

const uint8_t solid_tiles[5] = {0, 1, 2, 3, 4};
const int level1Width = 40; // TODO: change to real value
const int level1Height = 18; // TODO: change to real value

void loadMarioSprite(void);

void loadMarioSprite(void) {
    set_sprite_data(0, mario_TileLen, MarioTileLabel);
}

void level1_init(void) {
    DISPLAY_OFF;

    loadMarioSprite();
    fill_bkg_rect(0, 0, 20, 18, 127);
    levelHeight = level1Height;
    levelWidth = level1Width;
    wait_vbl_done();
    set_bkg_data(0, level1_TileLen, Level1TileLabel);
    set_bkg_submap(0, 0, 32, 18, level1TileMap, level1Width);
    wait_vbl_done();
    sprite_hide_all();
    mario_init(4, 14);

    SHOW_BKG;
    SHOW_SPRITES;

    DISPLAY_ON;
}

void level1_loop(void) {
    uint8_t keys = joypad();
    
    mario_update();
    wait_vbl_done();
}
