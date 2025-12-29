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
#include "hud.h"

const uint8_t solid_tiles[] = {0, 1, 2, 3, 4, 5 ,6 ,7 ,8 ,9 , 10, 11, 12 ,13 ,14 ,15 ,16 ,17, 32, 33 ,34 ,35, 35, 37, 38, 39, 40, 41, 42, 255};
const int level1Width = 80; // TODO: change to real value
const int level1Height = 32; // TODO: change to real value

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
    set_bkg_submap(0, 0, 32, 32, level1TileMap, level1Width);
    levelTileMap = level1TileMap;
    wait_vbl_done();
    sprite_hide_all();
    mario_init(4, 28);
    timer = 300;
    hud_init();

    SHOW_BKG;
    SHOW_SPRITES;

    DISPLAY_ON;
}

void level1_loop(void) {
    uint8_t keys = joypad();
    uint8_t moving = (keys & J_LEFT || keys & J_RIGHT);

    mario_update();
    hud_update();

    wait_vbl_done();

    move_bkg(camera_x, camera_y);
    mario_draw(moving);

    if ((camera_x >> 3) != (old_camera_x >> 3)) {
        if (camera_x > old_camera_x) {

            uint8_t column_to_draw = (camera_x >> 3) + 20;

            set_bkg_submap(column_to_draw, 0, 1, 32, levelTileMap, levelWidth);
        } else {
            uint8_t column_to_draw = (camera_x >> 3);
            set_bkg_submap(column_to_draw, 0, 1, 32, levelTileMap, levelWidth);
        }
    }
    old_camera_x = camera_x;
}
