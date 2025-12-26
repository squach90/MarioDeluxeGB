#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "selectFileMenu.h"

void main(void) {
    draw_selectFile();
    DISPLAY_OFF;
    fill_bkg_rect(0, 0, 20, 18, 40);
    HIDE_SPRITES;
    DISPLAY_ON;
}