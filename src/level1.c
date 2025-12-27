#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "marioSprite.h"
#include "selectFileMenu.h"
#include "mario.h"
#include "sprite.h"

void loadMarioSprite(void);

void loadMarioSprite(void) {
    set_sprite_data(0, mario_TileLen, MarioTileLabel);
}

void level1_init(void) {
    DISPLAY_OFF;

    loadMarioSprite();
    fill_bkg_rect(0, 0, 20, 18, 127);
    sprite_hide_all();
    mario_init(6, 8);

    SHOW_BKG;
    SHOW_SPRITES;

    DISPLAY_ON;
}

void level1_loop(void) {
    uint8_t keys = joypad();
    // TODO: logic here
    mario_update();
    wait_vbl_done();
}
