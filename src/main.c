#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "selectFileMenu.h"
#include "homeMenu.h"

void main(void) {
    draw_homeMenu();
    SHOW_BKG;
    DISPLAY_ON;

    uint8_t prev_keys = 0;

    while (1) {
        uint8_t keys = joypad();

        if ((keys & J_START) && !(prev_keys & J_START)) {
            break; // exit loop
        }

        prev_keys = keys;
        wait_vbl_done();
    }

    draw_selectFile();

    while (1) {
        wait_vbl_done();
    }
}
