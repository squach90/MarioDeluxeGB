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
const uint8_t special_pipes[] = {
    74, 24,   // First pipe (right side, row 24)
    0xFF, 0xFF  // End marker
};
const int level1Width = 80; // TODO: change to real value
const int level1Height = 32; // TODO: change to real value

uint8_t bump_timer = 0;
uint16_t bump_block_x, bump_block_y;
int16_t bump_offset_y = 0;
uint8_t bump_is_brick = 0;


void loadMarioSprite(void);

void loadMarioSprite(void) {
    set_sprite_data(0, mario_TileLen, MarioTileLabel);
}

void on_block_hit(uint16_t block_x, uint16_t block_y, uint8_t tile_id) {
    if (bump_timer > 0) return;

    bump_is_brick = (tile_id == 12 || tile_id == 13);

    if (bump_is_brick) {
        set_sprite_data(22, 1, &Level1TileLabel[12 * 16]);
        set_sprite_data(24, 1, &Level1TileLabel[13 * 16]);
    } else {
        level1TileMap[block_y * level1Width + block_x] = 14;
        level1TileMap[block_y * level1Width + block_x + 1] = 16;
        level1TileMap[(block_y + 1) * level1Width + block_x] = 15;
        level1TileMap[(block_y + 1) * level1Width + block_x + 1] = 17;

        set_sprite_data(18, 4, &Level1TileLabel[14 * 16]);
        coins++;
        score += 100;
    }

    uint8_t empty_tiles[4] = {127, 127, 127, 127};
    set_bkg_tiles(block_x % 32, block_y % 32, 2, 2, empty_tiles);

    bump_timer = 10;
    bump_block_x = block_x;
    bump_block_y = block_y;
    bump_offset_y = 0;
}


uint8_t is_special_pipe(uint16_t pipe_x, uint16_t pipe_y) {
    for (uint8_t i = 0; special_pipes[i] != 0xFF; i += 2) {
        if (special_pipes[i] == pipe_x && special_pipes[i + 1] == pipe_y) {
            return 1;
        }
    }
    return 0;
}

void on_pipe_enter(uint16_t pipe_x, uint16_t pipe_y) {
    if (is_special_pipe(pipe_x, pipe_y)) {
        coins++;
        score += 100;
        // TODO: load another map
    }
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
    set_sprite_data(18, 4, &Level1TileLabel[14 * 16]);  // import empty "? blocks" tiles in sprite section
    set_sprite_data(22, 1, &Level1TileLabel[12 * 16]); // top-left
    set_sprite_data(23, 1, &Level1TileLabel[13 * 16]); // bottom-right
    wait_vbl_done();
    sprite_hide_all();
    mario_init(4, 28);

    mario_set_block_callback(on_block_hit);
    mario_set_pipe_callback(on_pipe_enter);

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

    if (bump_timer > 0) {
        if (bump_timer > 5) bump_offset_y -= 2;
        else bump_offset_y += 2;

        int16_t sx = (bump_block_x * 8) - camera_x + 8;
        int16_t sy = (bump_block_y * 8) - camera_y + 16 + bump_offset_y;

        if (bump_is_brick) {
            // Brick
            set_sprite_tile(30, 22); // TL
            set_sprite_tile(31, 22); // TR
            set_sprite_tile(32, 23); // BL
            set_sprite_tile(33, 23); // BR
        } else {
            // ? block
            set_sprite_tile(30, 18);
            set_sprite_tile(31, 20);
            set_sprite_tile(32, 19);
            set_sprite_tile(33, 21);
        }


        move_sprite(30, sx, sy);
        move_sprite(31, sx + 8, sy);
        move_sprite(32, sx, sy + 8);
        move_sprite(33, sx + 8, sy + 8);

        bump_timer--;

        // When animation ends, hide sprites and draw the final tiles on BKG
        if (bump_timer == 0) {
            hide_sprite(30); hide_sprite(31); hide_sprite(32); hide_sprite(33);
            set_bkg_submap(bump_block_x, bump_block_y, 2, 2, level1TileMap, level1Width);
            bump_offset_y = 0;
        }
    }

    if (coins >= 100) {
        coins = 0;
        life++;
    }

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
