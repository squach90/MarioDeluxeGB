#include "mario.h"
#include "marioSprite.h"

#define MARIO_SPEED 1
#define MARIO_SPRITE_COUNT 4

static uint8_t mario_sprites[MARIO_SPRITE_COUNT];
static int16_t mario_x;
static int16_t mario_y;
static uint8_t facing_left = 0; // 0 = right, 1 = left

static uint8_t walk_frame = 0;
static uint8_t walk_counter = 0;
static uint8_t top_frame_index = 0;

// top-left and top-right frames
static const uint8_t top_left_frames[3]  = {4, 9, 10};
static const uint8_t top_right_frames[3] = {2, 2, 13};

// bottom tiles walk cycle (3 frames)
static const uint8_t bottom_bl_frames[3] = {5, 7, 11}; // bottom-left
static const uint8_t bottom_br_frames[3] = {6, 8, 12}; // bottom-right

void mario_init(uint8_t start_tile_x, uint8_t start_tile_y) {
    for (uint8_t i = 0; i < MARIO_SPRITE_COUNT; i++) {
        mario_sprites[i] = i; // use sprites 0–3
    }

    mario_x = start_tile_x * 8;
    mario_y = start_tile_y * 8;
}

static void mario_draw(uint8_t moving) {
    uint8_t tl, tr, bl, br;

    // bottom tiles (walking)
    if (moving) {
        bl = bottom_bl_frames[walk_frame];
        br = bottom_br_frames[walk_frame];
    } else {
        bl = 1; br = 3;
    }

    // top tiles
    tl = moving ? top_left_frames[top_frame_index] : 0;
    tr = moving ? top_right_frames[top_frame_index] : 2;

    if (facing_left) {
        // swap left/right tiles for flipping
        uint8_t tmp;

        tmp = tl; tl = tr; tr = tmp;
        tmp = bl; bl = br; br = tmp;
    }

    // set tiles
    set_sprite_tile(mario_sprites[0], tl);
    set_sprite_tile(mario_sprites[1], tr);
    set_sprite_tile(mario_sprites[2], bl);
    set_sprite_tile(mario_sprites[3], br);

    // flip the sprites
    uint8_t prop = facing_left ? S_FLIPX : 0;
    for (uint8_t i = 0; i < MARIO_SPRITE_COUNT; i++)
        set_sprite_prop(mario_sprites[i], prop);

    // move sprites
    move_sprite(mario_sprites[0], mario_x + 8,  mario_y + 16);
    move_sprite(mario_sprites[1], mario_x + 16, mario_y + 16);
    move_sprite(mario_sprites[2], mario_x + 8,  mario_y + 24);
    move_sprite(mario_sprites[3], mario_x + 16, mario_y + 24);
}


void mario_update(void) {
    uint8_t keys = joypad();
    uint8_t moving = 0;

    if (keys & J_LEFT)  { mario_x -= MARIO_SPEED; moving = 1; facing_left = 1; }
    if (keys & J_RIGHT) { mario_x += MARIO_SPEED; moving = 1; facing_left = 0; }
    if (keys & J_UP)    { mario_y -= MARIO_SPEED; moving = 1; }
    if (keys & J_DOWN)  { mario_y += MARIO_SPEED; moving = 1; }

    // update animation
    if (moving) {
        walk_counter++;
        if (walk_counter > 7) {
            walk_frame = (walk_frame + 1) % 3;
            top_frame_index = (top_frame_index + 1) % 3;
            walk_counter = 0;
        }
    } else {
        walk_frame = 0;
        top_frame_index = 0;
        walk_counter = 0;
    }

    // draw Mario once
    mario_draw(moving);
}
