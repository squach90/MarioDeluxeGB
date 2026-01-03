#ifndef GOOMBA_H
#define GOOMBA_H

#include <gb/gb.h>
#include <stdint.h>

#define GOOMBA_TILE_TOP 40   // Head
#define GOOMBA_TILE_L1  41   // Feet 1
#define GOOMBA_TILE_L2  42   // Feet 2

typedef struct {
    int16_t x;
    int16_t y;
    int8_t dir;         // -1: Left, 1: Right
    uint8_t alive;
    uint8_t anim_frame;
    uint8_t anim_timer;
} Goomba;

// Main func
void goomba_init(Goomba* g, uint16_t tile_x, uint16_t tile_y);
void goomba_update(Goomba* g);
void goomba_draw(Goomba* g, uint8_t first_sprite_id);

#endif