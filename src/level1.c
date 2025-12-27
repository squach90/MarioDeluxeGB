#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "mario.h"
#include "selectFileMenu.h"

void loadMarioSprite(void);

void loadMarioSprite(void) {
    set_sprite_data(0, mario_TileLen, MarioTileLabel);
}

void level1_init(void) {
    DISPLAY_OFF;

    loadMarioSprite();
    fill_bkg_rect(0, 0, 20, 18, 127);

    SHOW_BKG;
    SHOW_SPRITES;

    DISPLAY_ON;
}

void level1_loop(void) {
    uint8_t keys = joypad();
    // TODO: logic here
    wait_vbl_done();
}
