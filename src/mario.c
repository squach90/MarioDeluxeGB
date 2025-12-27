#include "mario.h"
#include "marioSprite.h"
#include <stdlib.h>
#include "level1Map.h"
#include "level1.h"

#define MARIO_SPRITE_COUNT 4
#define FP_SHIFT 8

// PHYSICS TUNING
#define GRAVITY 72
#define JUMP_POWER -1024
#define TERMINAL_VELOCITY 1152
#define MAX_JUMP_HOLD 15
#define JUMP_BOOST -40

#define MARIO_WALK_SPEED 1
#define MARIO_RUN_SPEED 2
#define ANIM_WALK_DELAY 7
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

static uint16_t old_camera_x = 0;

// Frames for walking animation
static const uint8_t top_left_frames[3]  = {4, 9, 10};
static const uint8_t top_right_frames[3] = {2, 2, 13};
static const uint8_t bottom_bl_frames[3] = {5, 7, 11};
static const uint8_t bottom_br_frames[3] = {6, 8, 12};

// Populate this with your solid tile IDs (e.g., {1, 2, 3})

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
    for(uint8_t i = 0; i < 5; i++) {
        if (tile_id == solid_tiles[i]) return 1;
    }
    return 0;
}

static void mario_draw(uint8_t moving) {
    uint8_t tl, tr, bl, br;

    int16_t screen_x = mario_x - camera_x;

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

    move_sprite(mario_sprites[0], screen_x + 8,  (mario_y >> FP_SHIFT) + 16 + 1);
    move_sprite(mario_sprites[1], screen_x + 16, (mario_y >> FP_SHIFT) + 16 + 1);
    move_sprite(mario_sprites[2], screen_x + 8,  (mario_y >> FP_SHIFT) + 24 + 1);
    move_sprite(mario_sprites[3], screen_x + 16, (mario_y >> FP_SHIFT) + 24 + 1);
}

void mario_update(void) {
    uint8_t keys = joypad();
    uint8_t newly_pressed = keys & (keys ^ previous_keys);
    uint8_t moving = 0;

    uint8_t current_speed = (keys & J_B) ? MARIO_RUN_SPEED : MARIO_WALK_SPEED;
    uint8_t current_anim_delay = (keys & J_B) ? ANIM_RUN_DELAY : ANIM_WALK_DELAY;

    if ((mario_y >> FP_SHIFT) > 160) {
        mario_init(0, 13); // Reset Mario to start position
    }

    // --- 1. HORIZONTAL MOVEMENT & WALL COLLISION ---
    int32_t next_x = mario_x;
    if (keys & J_LEFT) {
        next_x -= current_speed;
        moving = 1; facing_left = 1;
    } else if (keys & J_RIGHT) {
        next_x += current_speed;
        moving = 1; facing_left = 0;
    }

    if (moving) {
        // Check side collision (checking middle of Mario's height)
        int32_t side_x = (facing_left) ? (next_x + 4) : (next_x + 12);
        int32_t side_y = mario_y + (8 << FP_SHIFT); // Check center-waist height

        if (!is_solid(side_x << FP_SHIFT, side_y, level1TileMap)) {
            mario_x = next_x; // Only move X if not hitting a wall
        }
    }

    // --- 2. JUMP LOGIC ---
    if (on_ground) {
        if (newly_pressed & J_A) {
            mario_vy = JUMP_POWER;
            on_ground = 0;
            jump_hold_counter = 0;
        }
    } else if ((keys & J_A) && jump_hold_counter < MAX_JUMP_HOLD) {
        mario_vy += JUMP_BOOST;
        jump_hold_counter++;
    }

    // --- 3. GRAVITY ---
    int32_t current_gravity = GRAVITY;
    if (!on_ground && abs(mario_vy) < 150) current_gravity = GRAVITY / 2;

    mario_vy += current_gravity;
    if (mario_vy > TERMINAL_VELOCITY) mario_vy = TERMINAL_VELOCITY;

    if (mario_x > 80) {
        camera_x = mario_x - 80;
    } else {
        camera_x = 0;
    }
    if (camera_x > (levelWidth * 8) - 160) {
        camera_x = (levelWidth * 8) - 160;
    }
    move_bkg(camera_x, 0);

    if ((camera_x >> 3) != (old_camera_x >> 3)) {
        if (camera_x > old_camera_x) {

            uint8_t column_to_draw = (camera_x >> 3) + 20;

            set_bkg_submap(column_to_draw, 0, 1, 18, level1TileMap, levelWidth);
        } else {
            uint8_t column_to_draw = (camera_x >> 3);
            set_bkg_submap(column_to_draw, 0, 1, 18, level1TileMap, levelWidth);
        }
    }
    old_camera_x = camera_x;

    // --- 4. VERTICAL COLLISION ---
    mario_y += mario_vy;

    int32_t left_check = (mario_x + 4) << FP_SHIFT;
    int32_t right_check = (mario_x + 12) << FP_SHIFT;

    if (mario_vy < 0) { // Moving UP (Head check)
        if (is_solid(left_check, mario_y, level1TileMap) || 
            is_solid(right_check, mario_y, level1TileMap)) {
            mario_vy = 0;
            mario_y = (int32_t)(((mario_y >> FP_SHIFT) / 8 + 1) * 8) << FP_SHIFT;
        }
    } else { // Moving DOWN (Feet check)
        int32_t feet_y = mario_y + (16 << FP_SHIFT);
        if (is_solid(left_check, feet_y, level1TileMap) || 
            is_solid(right_check, feet_y, level1TileMap)) {
            mario_vy = 0;
            on_ground = 1;
            mario_y = (int32_t)(((feet_y >> FP_SHIFT) / 8) * 8 - 16) << FP_SHIFT;
        } else {
            on_ground = 0;
        }
    }

    // --- 5. ANIMATION ---
    if (moving && on_ground) {
        if (++walk_counter > current_anim_delay) {
            walk_frame = (walk_frame + 1) % 3;
            top_frame_index = (top_frame_index + 1) % 3;
            walk_counter = 0;
        }
    } else if (on_ground) {
        walk_frame = 0; top_frame_index = 0; walk_counter = 0;
    }

    mario_draw(moving);
    previous_keys = keys;
}