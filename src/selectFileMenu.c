#include "selectFile.h"
#include "selectFileMenu.h"
#include "selectFileMap.h"
#include "selectFileSprite.h"
#include "main.h"
#include "sprite.h"

uint8_t saveIndex = 0;

static uint8_t marioSprites[8];

void selectFileMario_init(void) {
    sprite_pool_init();

    for (uint8_t i = 0; i < 8; i++) {
        marioSprites[i] = sprite_alloc();
    }
}

static uint8_t marioPipeIndex;
static uint8_t prev_keys;

void draw_selectFileMario(uint8_t tile_x, uint8_t tile_y) {
    place_sprite_px(0, tile_x,     tile_y + 1, 0, 1, 0);
    place_sprite_px(1, tile_x,     tile_y + 2, 0, 1, 1);
    place_sprite_px(2, tile_x + 1, tile_y + 1, 0, 1, 2);
    place_sprite_px(3, tile_x + 1, tile_y + 2, 0, 1, 3);
    place_sprite_px(4, tile_x,     tile_y + 3, 0, 1, 4);
    place_sprite_px(5, tile_x,     tile_y + 4, 0, 1, 5);
    place_sprite_px(6, tile_x + 1, tile_y + 3, 0, 1, 6);
    place_sprite_px(7, tile_x + 1, tile_y + 4, 0, 1, 7);
}

void selectFile_init(void) {
    DISPLAY_OFF;

    marioPipeIndex = 0;
    prev_keys = 0;

    set_bkg_data(0, selectFile_TileLen, selectFileTile);
    set_bkg_tiles(0, 0, 20, 18, selectFileTileMap);
    set_sprite_data(0, selectFileSprite_TileLen, selectFileSpriteTile);

    selectFileMario_init();
    sprite_hide_all();

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

uint8_t selectFile_loop(void) {
    uint8_t keys = joypad();

    if ((keys & J_RIGHT) && !(prev_keys & J_RIGHT)) {
        marioPipeIndex = (marioPipeIndex + 1) % 3;
    }

    if ((keys & J_LEFT) && !(prev_keys & J_LEFT)) {
        marioPipeIndex = (marioPipeIndex == 0) ? 2 : marioPipeIndex - 1;
    }

    if ((keys & J_A) && !(prev_keys & J_A)) {
        saveIndex = marioPipeIndex;
        HIDE_SPRITES;
        return MENU_GAME;
    }

    if ((keys & J_B) && !(prev_keys & J_B)) {
        HIDE_SPRITES;
        return MENU_HOME;
    }

    switch (marioPipeIndex) {
        case 0: draw_selectFileMario(3, 7);  break;
        case 1: draw_selectFileMario(9, 7);  break;
        case 2: draw_selectFileMario(15, 7); break;
    }

    prev_keys = keys;
    wait_vbl_done();

    return MENU_FILESELECT;
}
