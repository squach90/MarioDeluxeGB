#include "goomba.h"
#include "main.h"
#include "mario.h"
#include <stdlib.h>

void goomba_init(Goomba* g, uint16_t tile_x, uint16_t tile_y) {
    g->x = tile_x * 8;
    g->y = tile_y * 8;
    g->dir = -1; 
    g->alive = 1;
    g->anim_frame = 0;
    g->anim_timer = 0;
}

void goomba_update(Goomba* g) {
    if (!g->alive) return;

    if (g->anim_timer % 2 == 0) {
        int16_t next_x = g->x + g->dir;
        int32_t check_x = (g->dir == -1) ? (next_x) : (next_x + 15);
        if (is_solid((int32_t)check_x << 8, (int32_t)(g->y + 8) << 8, levelTileMap)) {
            g->dir *= -1;
        } else {
            g->x = next_x;
        }
    }

    // 3. ANIMATION
    g->anim_timer++;
    if (g->anim_timer > 10) {
        g->anim_frame = !g->anim_frame;
        g->anim_timer = 0;
    }

    // 4. Collision with Mario
    int16_t mx = (int16_t)mario_x;
    int16_t my = (int16_t)(mario_y >> 8);

    // collision box Mario : mx+4 à mx+12 | Goomba : g->x à g->x+15
    if (mx + 12 > g->x && mx + 4 < g->x + 16) {
        // Mario est au dessus et tombe
        if (my + 16 >= g->y && my + 16 <= g->y + 8 && mario_vy > 0) {
            g->alive = 0;
            mario_vy = -512; // medium bounce
            score += 200;
        }

        // Mario touch the side (damage)
        else if (my + 16 > g->y + 4 && my < g->y + 16) {
            // TODO: live -1
        }
    }
}

void goomba_draw(Goomba* g, uint8_t first_sprite_id) {
    int16_t sx = g->x - camera_x + 8;
    int16_t sy = g->y - camera_y + 16;

    if (!g->alive || sx < 0 || sx > 168 || sy < 0 || sy > 152) {
        for(uint8_t i=0; i<4; i++) move_sprite(first_sprite_id + i, 0, 0);
        return;
    }

    set_sprite_tile(first_sprite_id,     GOOMBA_TILE_TOP);
    set_sprite_tile(first_sprite_id + 1, GOOMBA_TILE_TOP);
    set_sprite_prop(first_sprite_id,     0);
    set_sprite_prop(first_sprite_id + 1, S_FLIPX);

    if (g->anim_frame == 0) {
        set_sprite_tile(first_sprite_id + 2, GOOMBA_TILE_L1); // Little Feet
        set_sprite_prop(first_sprite_id + 2, 0);

        set_sprite_tile(first_sprite_id + 3, GOOMBA_TILE_L2); // Big Feet
        set_sprite_prop(first_sprite_id + 3, 0);
    } else {
        set_sprite_tile(first_sprite_id + 2, GOOMBA_TILE_L2); // Big Feet
        set_sprite_prop(first_sprite_id + 2, S_FLIPX);

        set_sprite_tile(first_sprite_id + 3, GOOMBA_TILE_L1); // Little Feet
        set_sprite_prop(first_sprite_id + 3, S_FLIPX);
    }

    move_sprite(first_sprite_id,     sx,     sy);      // Head Left
    move_sprite(first_sprite_id + 1, sx + 8, sy);      // Head Right
    move_sprite(first_sprite_id + 2, sx,     sy + 8);  // Feet Left
    move_sprite(first_sprite_id + 3, sx + 8, sy + 8);  // Feet Right
}