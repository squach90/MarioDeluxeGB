#include "mario.h"
#include "marioSprite.h"
#include <stdlib.h>

#define MARIO_SPRITE_COUNT 4
#define FP_SHIFT 8
#define GRAVITY 72               // 0.28125 * 256
#define JUMP_POWER -1024         // -4.0 * 256
#define TERMINAL_VELOCITY 1152   // 4.5 * 256
#define MAX_JUMP_HOLD 15         // Frames allowed to "boost" the jump
#define JUMP_BOOST -40           // Upward force applied while holding A

#define MARIO_SPEED 1            // Horizontal speed (1 pixel per frame)
#define GROUND_Y (120 << FP_SHIFT)

#define JUMP_TL 14
#define JUMP_TR 15
#define JUMP_BL 17
#define JUMP_BR 16

#define MARIO_WALK_SPEED 1
#define MARIO_RUN_SPEED 2
#define ANIM_WALK_DELAY 7
#define ANIM_RUN_DELAY 3

static uint8_t mario_sprites[MARIO_SPRITE_COUNT];
static int32_t mario_x;      // keep as int for now
static int32_t mario_y;      // fixed point
static int32_t mario_vy = 0;

static uint8_t facing_left = 0;
static uint8_t on_ground = 0;
static uint8_t jump_hold_counter = 0;

static uint8_t walk_frame = 0;
static uint8_t walk_counter = 0;
static uint8_t top_frame_index = 0;

static uint8_t previous_keys = 0;

// frames
static const uint8_t top_left_frames[3]  = {4, 9, 10};
static const uint8_t top_right_frames[3] = {2, 2, 13};
static const uint8_t bottom_bl_frames[3] = {5, 7, 11};
static const uint8_t bottom_br_frames[3] = {6, 8, 12};

void mario_init(uint8_t start_tile_x, uint8_t start_tile_y) {
    for (uint8_t i = 0; i < MARIO_SPRITE_COUNT; i++)
        mario_sprites[i] = i;

    mario_x = start_tile_x * 8;
    mario_y = start_tile_y * 8 << FP_SHIFT; // convert to fixed point
    mario_vy = 0;
    on_ground = 0;
    jump_hold_counter = 0;
}

static void mario_draw(uint8_t moving) {
    uint8_t tl, tr, bl, br;

    if (!on_ground) {
        tl = JUMP_TL;
        tr = JUMP_TR;
        bl = JUMP_BL;
        br = JUMP_BR;
    } else if (moving) {
        bl = bottom_bl_frames[walk_frame];
        br = bottom_br_frames[walk_frame];
        tl = top_left_frames[top_frame_index];
        tr = top_right_frames[top_frame_index];
    } else {
        tl = 0; tr = 2;
        bl = 1; br = 3;
    }

    if (facing_left) {
        uint8_t tmp;
        tmp = tl; tl = tr; tr = tmp;
        tmp = bl; bl = br; br = tmp;
    }

    set_sprite_tile(mario_sprites[0], tl);
    set_sprite_tile(mario_sprites[1], tr);
    set_sprite_tile(mario_sprites[2], bl);
    set_sprite_tile(mario_sprites[3], br);

    uint8_t prop = facing_left ? S_FLIPX : 0;
    for (uint8_t i = 0; i < MARIO_SPRITE_COUNT; i++)
        set_sprite_prop(mario_sprites[i], prop);

    move_sprite(mario_sprites[0], mario_x + 8,  (mario_y >> FP_SHIFT) + 16);
    move_sprite(mario_sprites[1], mario_x + 16, (mario_y >> FP_SHIFT) + 16);
    move_sprite(mario_sprites[2], mario_x + 8,  (mario_y >> FP_SHIFT) + 24);
    move_sprite(mario_sprites[3], mario_x + 16, (mario_y >> FP_SHIFT) + 24);
}

void mario_update(void) {
    uint8_t keys = joypad();
    uint8_t newly_pressed = keys & (keys ^ previous_keys);
    uint8_t moving = 0;

    uint8_t current_speed = MARIO_WALK_SPEED;
    uint8_t current_anim_delay = ANIM_WALK_DELAY;

    if (keys & J_B) {
        current_speed = MARIO_RUN_SPEED;
        current_anim_delay = ANIM_RUN_DELAY;
    }

    if (keys & J_LEFT)  { mario_x -= current_speed; moving = 1; facing_left = 1; }
    if (keys & J_RIGHT) { mario_x += current_speed; moving = 1; facing_left = 0; }

    // jump logic
    if (on_ground) {
        if (newly_pressed & J_A) {
            mario_vy = JUMP_POWER;
            on_ground = 0;
            jump_hold_counter = 0;
        }
    } else {
        if ((keys & J_A) && jump_hold_counter < MAX_JUMP_HOLD) {
            mario_vy += JUMP_BOOST;
            jump_hold_counter++;
        } else {
            jump_hold_counter = MAX_JUMP_HOLD;
        }
    }

    int32_t current_gravity = GRAVITY;

    if (!on_ground && labs(mario_vy) < 150) {
        current_gravity = GRAVITY / 2;
    }

    // apply gravity
    mario_vy += current_gravity;
    if (mario_vy > TERMINAL_VELOCITY) {
        mario_vy = TERMINAL_VELOCITY;
    }

    mario_y += mario_vy;

    // collision with ground
    if (mario_y >= GROUND_Y) {
        mario_y = GROUND_Y;
        mario_vy = 0;
        on_ground = 1;
    }

    // animation
    if (moving) {
        walk_counter++;
        if (walk_counter > current_anim_delay) {
            walk_frame = (walk_frame + 1) % 3;
            top_frame_index = (top_frame_index + 1) % 3;
            walk_counter = 0;
        }
    } else {
        walk_frame = 0;
        top_frame_index = 0;
        walk_counter = 0;
    }

    mario_draw(moving);
    previous_keys = keys;
}
