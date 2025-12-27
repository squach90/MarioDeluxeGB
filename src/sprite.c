#include "sprite.h"

static uint8_t sprite_count = 0;

void sprite_pool_init(void) {
    sprite_count = 0;
}

uint8_t sprite_alloc(void) {
    if (sprite_count >= MAX_SPRITES) return 0;
    return sprite_count++;
}

void sprite_hide_all(void) {
    for (uint8_t i = 0; i < MAX_SPRITES; i++) {
        move_sprite(i, 0, 0); // off-screen
    }
}

void place_sprite_px(
    uint8_t sprite_id,
    uint8_t tile_x,
    uint8_t tile_y,
    int8_t offset_x,
    int8_t offset_y,
    uint8_t tile_index
) {
    set_sprite_tile(sprite_id, tile_index);
    move_sprite(
        sprite_id,
        tile_x * 8 + offset_x + 8,
        tile_y * 8 + offset_y + 16
    );
}