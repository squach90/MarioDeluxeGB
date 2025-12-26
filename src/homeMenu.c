#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Logo.h"
#include "logoMap.h"

void draw_homeMenu(void);

void draw_homeMenu(void) {
    set_bkg_data(0, logo_TileLen, LogoTileLabel);
    set_bkg_tiles(0, 0, 20, 18, logoTileMap);
}