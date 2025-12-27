#ifndef MARIO_H
#define MARIO_H

#include <gb/gb.h>
#include <stdint.h>

void mario_init(uint8_t start_tile_x, uint8_t start_tile_y);
void mario_update(void);
extern const uint8_t solid_tiles[5];

#endif
