#include "gfx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

void print_gfx(uint8_t gfx[]) {
    for (int i=0; i < 2048; i++) {
        if (i % 64 == 0 && i != 0) {
            printf("\n");
        }
        if (gfx[i] == 0) {
            printf(" ");
        } else if (gfx[i] == 1) {
            printf("X");
        }
    };
}

int draw_gfx(uint8_t gfx[], uint8_t x, uint8_t y, uint8_t n, uint8_t* index_register_ptr_on_memory) {
    x = x % 64;
    y = y % 32;

    int pos = get_gfx_pos(x, y);
    int collide = 0;

    for (short row = 0; row < n; row++) {
        if (y + row >= 32) break;
        uint8_t spriteByte = *(index_register_ptr_on_memory + row);
        for (short bit = 0; bit < 8; bit++) {
            if (x + bit >= 64) break;
            // 0x80 = 1000 0000 com o shift de bit temos o 1 no lugar do bit atual
            // daí é só fazer um and pra poder pegar só se é 1 ou 0 no lugar
            uint8_t spritePixel = (spriteByte & (0x80 >> bit)) >> (7 - bit);
            uint8_t* gfxPixel = &gfx[get_gfx_pos(x + bit, y + row)];

            if (spritePixel) {
                if (*gfxPixel == 1) {
                    collide = 1;
                };

                *gfxPixel ^= 1;
            }
        }
    }

    return collide;
}

int get_gfx_pos(uint8_t x, uint8_t y) {
    return x + (y * 64);
}

void clear_gfx(uint8_t gfx[]) {
    memset(gfx, 0, 2048);
}