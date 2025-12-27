#include <gb/gb.h>
#include <stdint.h>
#include "Logo.h"
#include "logoMap.h"

void homeMenu_init(void) {
    DISPLAY_OFF;

    set_bkg_data(0, logo_TileLen, LogoTileLabel);
    set_bkg_tiles(0, 0, 20, 18, logoTileMap);

    SHOW_BKG;
    DISPLAY_ON;
}
