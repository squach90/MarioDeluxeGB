#ifndef HUD_H
#define HUD_H

#include <stdint.h>

void hud_init(void);
void hud_update(void);
void hud_print_num(uint8_t x, uint8_t y, uint8_t digits, int value);

#endif