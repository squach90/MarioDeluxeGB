#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "selectFile.h"
#include "selectFileMenu.h"
#include "selectFileMap.h"
#include "selectFileSprite.h"
#include "main.h"

// Draw background
void draw_selectFile(void);

void draw_selectFileMario(uint8_t tile_x, uint8_t tile_y, int8_t offset_x, int8_t offset_y);

void place_sprite_px(uint8_t sprite_id, uint8_t tile_x, uint8_t tile_y, int8_t offset_x, int8_t offset_y, uint8_t tile_index) {
    set_sprite_tile(sprite_id, tile_index);
    // Add 16 pixels to Y for Game Boy sprite alignment
    move_sprite(sprite_id, tile_x * 8 + offset_x, tile_y * 8 + offset_y + 16);
}

void draw_selectFileMario(uint8_t tile_x, uint8_t tile_y, int8_t offset_x, int8_t offset_y) {
    set_sprite_data(0, selectFileSprite_TileLen, selectFileSpriteTile);

    // Mario is 2 wide x 4 tall (8 tiles)
    place_sprite_px(0, tile_x,     tile_y + 1, offset_x, offset_y, 0);
    place_sprite_px(1, tile_x,     tile_y + 2, offset_x, offset_y, 1);
    place_sprite_px(2, tile_x + 1, tile_y + 1, offset_x, offset_y, 2);
    place_sprite_px(3, tile_x + 1, tile_y + 2, offset_x, offset_y, 3);
    place_sprite_px(4, tile_x,     tile_y + 3, offset_x, offset_y, 4);
    place_sprite_px(5, tile_x,     tile_y + 4, offset_x, offset_y, 5);
    place_sprite_px(6, tile_x + 1, tile_y + 3, offset_x, offset_y, 6);
    place_sprite_px(7, tile_x + 1, tile_y + 4, offset_x, offset_y, 7);
}

void draw_selectFile(void) {
    uint8_t marioPipeIndex = 0;

    set_bkg_data(0, selectFile_TileLen, selectFileTile);
    fill_bkg_rect(0, 0, 20, 18, 14);
    set_bkg_tiles(0, 0, 20, 18, selectFileTileMap);

    SHOW_BKG;
    SHOW_SPRITES;

    uint8_t prev_keys = 0;

    while (1) {
        uint8_t keys = joypad();

        if ((keys & J_RIGHT) && !(prev_keys & J_RIGHT)) {
            marioPipeIndex = (marioPipeIndex + 1) % 3;
        }

        if ((keys & J_LEFT) && !(prev_keys & J_LEFT)) {
            if (marioPipeIndex == 0)
                marioPipeIndex = 2;
            else
                marioPipeIndex--;
        }

        if (keys & J_START) {
            saveIndex = marioPipeIndex;
            HIDE_SPRITES;
            return;
        }

        prev_keys = keys;

        switch (marioPipeIndex) {
            case 0: draw_selectFileMario(3, 7, 9, 1); break;
            case 1: draw_selectFileMario(9, 7, 9, 1); break;
            case 2: draw_selectFileMario(15, 7, 9, 1); break;
        }

        wait_vbl_done();
    }
}
