#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "selectFileMenu.h"
#include "homeMenu.h"
#include "main.h"

void main(void) {
    draw_homeMenu();
    SHOW_BKG;
    DISPLAY_ON;

    uint8_t state = MENU_HOME;
    uint8_t prev_keys = 0;

    while (1) {
        switch (state) {

            case MENU_HOME: {
                draw_homeMenu();
                wait_vbl_done();
                DISPLAY_ON;

                while (1) {
                    uint8_t keys = joypad();
                    if ((keys & J_START) && !(prev_keys & J_START)) {
                        state = MENU_FILESELECT;
                        break;
                    }
                    prev_keys = keys;
                    wait_vbl_done();
                }
                break;
            }

            case MENU_FILESELECT:
                state = draw_selectFile();
                break;

            case MENU_GAME:
                // TODO: launch Game
                break;
        }
    }
}
