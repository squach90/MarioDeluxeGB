#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "selectFileMenu.h"
#include "homeMenu.h"

void main(void) {
    SHOW_BKG;
    DISPLAY_ON;
    draw_homeMenu();
    // draw_selectFile();
    // DISPLAY_OFF;
    // fill_bkg_rect(0, 0, 20, 18, 40);
    // DISPLAY_ON;
}