#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

void enable_raw_mode();
void disable_raw_mode();
void update_input(uint8_t* keypad);

#endif // INPUT_H