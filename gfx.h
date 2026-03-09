#ifndef GFX_H
#define GFX_H
#include <stdint.h>

void print_gfx(uint8_t gfx[]);
void gfx_to_print_buffer(uint8_t gfx[], char* print_buffer);
int draw_gfx(uint8_t gfx[], uint8_t x, uint8_t y, uint8_t n, uint8_t* index_register_ptr_on_memory);
int get_gfx_pos(uint8_t x, uint8_t y);
void clear_gfx(uint8_t gfx[]);

#endif // GFX_H