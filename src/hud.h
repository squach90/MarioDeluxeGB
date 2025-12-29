#ifndef HUD_H
#define HUD_H

#include <stdint.h>

// Call this once at the start of any level
void hud_init(void);

// Call this in your main loop to refresh numbers
void hud_update(void);

// Helper to print numbers anywhere on the Window
void hud_print_num(uint8_t x, uint8_t y, uint8_t digits, int value);

#endif