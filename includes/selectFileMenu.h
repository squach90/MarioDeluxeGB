#ifndef SELECTFILEMENU_H
#define SELECTFILEMENU_H

#include <stdint.h>

void selectFile_init(void);
uint8_t selectFile_loop(void);

extern uint8_t saveIndex;

#endif
