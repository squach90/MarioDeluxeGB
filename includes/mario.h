#ifndef MARIO_H
#define MARIO_H

#include <gb/gb.h>
#include <stdint.h>

void mario_init(uint8_t start_tile_x, uint8_t start_tile_y);
void mario_draw(uint8_t moving);
void mario_update(void);

typedef void (*block_hit_callback)(uint16_t tile_x, uint16_t tile_y, uint8_t tile_id);
void mario_set_block_callback(block_hit_callback callback);

typedef void (*pipe_enter_callback)(uint16_t tile_x, uint16_t tile_y);
void mario_set_pipe_callback(pipe_enter_callback callback);

extern uint16_t camera_x;
extern uint16_t old_camera_x;

extern uint16_t camera_y;
extern uint16_t old_camera_y;

#endif
