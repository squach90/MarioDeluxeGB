#ifndef SPRITE_H
#define SPRITE_H

#include <gb/gb.h>
#include <stdint.h>

#define MAX_SPRITES 40

void sprite_pool_init(void);
uint8_t sprite_alloc(void);
void sprite_hide_all(void);

void place_sprite_px(
    uint8_t sprite_id,
    uint8_t tile_x,
    uint8_t tile_y,
    int8_t offset_x,
    int8_t offset_y,
    uint8_t tile_index
);

#endif
