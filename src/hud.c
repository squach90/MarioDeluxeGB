#include <gb/gb.h>
#include <stdint.h>
#include "main.h"
#include "windowSprite.h"

#define HUD_TILE_OFFSET 100

static int last_score = -1;
static int last_coins = -1;
static int last_timer = -1;

void hud_print_num(uint8_t x, uint8_t y, uint8_t digits, int value) {
    for (int8_t i = digits - 1; i >= 0; i--) {
        set_win_tile_xy(x + i, y, (uint8_t)(value % 10) + 3 + HUD_TILE_OFFSET);
        value /= 10;
    }
}

void hud_init(void) {
    set_win_data(HUD_TILE_OFFSET, window_TileLen, WindowTileLabel);

    fill_win_rect(0, 0, 20, 1, HUD_TILE_OFFSET + 100); 

    set_win_tile_xy(9, 0, HUD_TILE_OFFSET + 2);  // Coin
    set_win_tile_xy(10, 0, HUD_TILE_OFFSET + 0);
    set_win_tile_xy(16, 0, HUD_TILE_OFFSET + 1); // Time

    last_score = -1;
    last_coins = -1;
    last_timer = -1;

    move_win(7, 136);
    SHOW_WIN;
}

void hud_update(void) {
    if (sys_time % 60 == 0 && timer > 0) {
        timer--;
    }

    if (score != last_score) {
        hud_print_num(1, 0, 5, score);
        last_score = score;
    }

    if (coins != last_coins) {
        hud_print_num(11, 0, 2, coins);
        last_coins = coins;
    }

    if (timer != last_timer) {
        hud_print_num(17, 0, 3, timer);
        last_timer = timer;
    }

    if (timer == 0) {
        // TODO: dead
    }
}