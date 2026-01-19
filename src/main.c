#include <gb/gb.h>
#include <stdint.h>

#include "main.h"
#include "homeMenu.h"
#include "selectFileMenu.h"
#include "level1.h"

uint16_t levelWidth = NULL;
uint8_t levelHeight = NULL;
const uint8_t * levelTileMap = NULL;
int score = 0;
int coins = 0;
int timer = NULL;
int life = 5;
int is_paused = 0;
uint8_t blocks_used[128][16];

void main(void) {
    uint8_t state = MENU_HOME;
    uint8_t prev_state = 255; // valeur impossible pour forcer init

    while (1) {

        if (state != prev_state) {
            switch (state) {

                case MENU_HOME:
                    homeMenu_init();
                    HIDE_SPRITES;
                    break;

                case MENU_FILESELECT:
                    selectFile_init();
                    break;

                case MENU_GAME:
                    level1_init();
                    break;
            }
            prev_state = state;
        }

        switch (state) {

            case MENU_HOME: {
                static uint8_t prev_keys = 0;
                uint8_t keys = joypad();

                if ((keys & J_START) && !(prev_keys & J_START)) {
                    state = MENU_FILESELECT;
                }

                prev_keys = keys;
                wait_vbl_done();
                break;
            }

            case MENU_FILESELECT:
                state = selectFile_loop();
                break;

            case MENU_GAME:
                level1_loop();
                break;
        }
    }
}
