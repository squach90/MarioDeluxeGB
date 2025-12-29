#include "mario.h"
#include "marioSprite.h"
#include <stdlib.h>
#include "main.h"

#define MARIO_SPRITE_COUNT 4
#define FP_SHIFT 8

// PHYSICS TUNING
#define GRAVITY 60
#define JUMP_POWER -1024
#define TERMINAL_VELOCITY 1152
#define MAX_JUMP_HOLD 15
#define JUMP_BOOST -40

#define MARIO_WALK_SPEED 1
#define MARIO_RUN_SPEED 2
#define ANIM_WALK_DELAY 8
#define ANIM_RUN_DELAY 3

// SPRITE IDS
#define JUMP_TL 14
#define JUMP_TR 15
#define JUMP_BL 17
#define JUMP_BR 16

static uint8_t mario_sprites[MARIO_SPRITE_COUNT];
static int32_t mario_x;
static int32_t mario_y;
static int32_t mario_vy = 0;

static uint8_t facing_left = 0;
static uint8_t on_ground = 0;
static uint8_t jump_hold_counter = 0;

static uint8_t walk_frame = 0;
static uint8_t walk_counter = 0;
static uint8_t top_frame_index = 0;
static uint8_t previous_keys = 0;

uint16_t camera_x = 0;
uint16_t camera_y = 0;

uint16_t old_camera_x = 0;
uint16_t old_camera_y = 0;

// Frames for walking animation
static const uint8_t top_left_frames[3]  = {4, 9, 10};
static const uint8_t top_right_frames[3] = {2, 2, 13};
static const uint8_t bottom_bl_frames[3] = {5, 7, 11};
static const uint8_t bottom_br_frames[3] = {6, 8, 12};

void mario_init(uint8_t start_tile_x, uint8_t start_tile_y) {
    for (uint8_t i = 0; i < MARIO_SPRITE_COUNT; i++)
        mario_sprites[i] = i;

    mario_x = (int32_t)start_tile_x * 8;
    mario_y = (int32_t)start_tile_y * 8 << FP_SHIFT;
    mario_vy = 0;
    on_ground = 0;
}

uint8_t is_solid(int32_t x_fp, int32_t y_fp, const uint8_t *data) {
    uint16_t tx = (x_fp >> FP_SHIFT) / 8;
    uint16_t ty = (y_fp >> FP_SHIFT) / 8;

    if (tx >= levelWidth || ty >= levelHeight) return 0;
    if (y_fp < 0) return 0;

    uint8_t tile_id = data[ty * levelWidth + tx];
    for(uint8_t i = 0; solid_tiles[i] != 255; i++) {
        if (tile_id == solid_tiles[i]) return 1;
    }
    return 0;
}

void mario_draw(uint8_t moving) {
    uint8_t tl, tr, bl, br;

    int16_t screen_x = mario_x - camera_x;
    int16_t screen_y = (mario_y >> FP_SHIFT) - camera_y;

    if (!on_ground) {
        tl = JUMP_TL; tr = JUMP_TR; bl = JUMP_BL; br = JUMP_BR;
    } else if (moving) {
        bl = bottom_bl_frames[walk_frame]; br = bottom_br_frames[walk_frame];
        tl = top_left_frames[top_frame_index]; tr = top_right_frames[top_frame_index];
    } else {
        tl = 0; tr = 2; bl = 1; br = 3;
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

    move_sprite(mario_sprites[0], screen_x + 8,  screen_y + 16);
    move_sprite(mario_sprites[1], screen_x + 16, screen_y + 16);
    move_sprite(mario_sprites[2], screen_x + 8,  screen_y + 24);
    move_sprite(mario_sprites[3], screen_x + 16, screen_y + 24);
}

void mario_update(void) {
    uint8_t keys = joypad();
    uint8_t newly_pressed = keys & (keys ^ previous_keys);
    uint8_t moving = 0;

    uint8_t current_speed = (keys & J_B) ? MARIO_RUN_SPEED : MARIO_WALK_SPEED;
    uint8_t current_anim_delay = (keys & J_B) ? ANIM_RUN_DELAY : ANIM_WALK_DELAY;

    // 1. DEAD / RESET
    if ((mario_y >> FP_SHIFT) > (levelHeight * 8)) {
        mario_init(4, 2); 
        return;
    }

    // 2. HORIZONTAL MOUVEMENT
    int32_t next_x = mario_x;
    if (keys & J_LEFT) {
        next_x -= current_speed;
        facing_left = 1; moving = 1;
    } else if (keys & J_RIGHT) {
        next_x += current_speed;
        facing_left = 0; moving = 1;
    }

    if (moving) {
        int32_t check_x = (facing_left) ? (next_x + 4) : (next_x + 12);
        int32_t head_y = (mario_y >> FP_SHIFT) + 4;
        int32_t foot_y = (mario_y >> FP_SHIFT) + 14;

        if (!is_solid(check_x << FP_SHIFT, head_y << FP_SHIFT, levelTileMap) &&
            !is_solid(check_x << FP_SHIFT, foot_y << FP_SHIFT, levelTileMap)) {
            mario_x = next_x;
        } else {
            moving = 0;
        }
    }

    // 3. JUMP & GRAVITY
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
        }
        
        int32_t current_gravity = GRAVITY;
        if (labs(mario_vy) < 150) current_gravity = GRAVITY / 2;
        mario_vy += current_gravity;
        
        if (mario_vy > TERMINAL_VELOCITY) mario_vy = TERMINAL_VELOCITY;
    }

    // 4. VERTICAL PHYSICS  & COLLISIONS
    mario_y += mario_vy;

    int32_t left_check = (mario_x + 4) << FP_SHIFT;
    int32_t right_check = (mario_x + 12) << FP_SHIFT;

    if (mario_vy < 0) {
        if (is_solid(left_check, mario_y, levelTileMap) ||
            is_solid(right_check, mario_y, levelTileMap)) {
            mario_vy = 0;
            mario_y = (int32_t)(((mario_y >> FP_SHIFT) / 8 + 1) * 8) << FP_SHIFT;
        }
    } else {
        int32_t feet_y = mario_y + (16 << FP_SHIFT);
        if (is_solid(left_check, feet_y, levelTileMap) ||
            is_solid(right_check, feet_y, levelTileMap)) {
            mario_vy = 0;
            on_ground = 1;
            mario_y = (int32_t)(((feet_y >> FP_SHIFT) / 8) * 8 - 16) << FP_SHIFT;
        } else {
            on_ground = 0;
        }
    }

    // 5. CAMERA SYNC
    int32_t final_py = mario_y >> FP_SHIFT;
    
    // Camera X
    if (mario_x > 80) camera_x = mario_x - 80;
    else camera_x = 0;
    uint16_t max_sx = (levelWidth * 8) - 160;
    if (camera_x > max_sx) camera_x = max_sx;

    // Camera Y
    if (final_py > 72) camera_y = final_py - 72;
    else camera_y = 0;
    int16_t max_sy = (levelHeight * 8) - 144;
    if (camera_y > max_sy) camera_y = max_sy;

    // 6. ANIMATION
    if (moving && on_ground) {
        if (++walk_counter > current_anim_delay) {
            walk_frame = (walk_frame + 1) % 3;
            top_frame_index = (top_frame_index + 1) % 3;
            walk_counter = 0;
        }
    } else if (on_ground) {
        walk_frame = 0; top_frame_index = 0; walk_counter = 0;
    }

    previous_keys = keys;
}