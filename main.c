#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50

void reset_cursor() {
    printf("\033[H\033[J");
}

int main() {
    FILE *file = fopen("IBM Logo.ch8", "rb");

    if (file == NULL) {
        printf("Erro lendo IBM Logo.ch8\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    uint8_t* buffer = malloc(size);

    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(uint8_t), size, file);
    fclose(file);

    // chip-8 variables
    uint8_t memory[4096] = {0};
    uint16_t stack[16] = {0};
    uint8_t sp = 0;
    uint8_t registers[16] = {0};
    uint16_t pc = START_ADDRESS; 
    uint8_t gfx[64 * 32] = {0};
    uint16_t index_register = 0;

    if (size > 4096 - START_ADDRESS) {
        printf("ROM maior que o espaço disponível na memória\n");
        return 1;
    }

    uint8_t fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // 80 = tamanho do fontset
    for (int i = 0; i < 80; i++) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    for (int i = 0; i < size; i++) {
        memory[START_ADDRESS + i] = buffer[i];
    }

    free(buffer);

    for (;;) {
        // fetch
        // pega os dois bytes na memória, que equivalem a 2*8 = 16 bits, um opcode hexadecimal
        uint16_t opcode = (memory[pc] << 8) | memory[pc+1]; 

        /*printf("DEBUG: OPCODE -- Binário: ");
        for (int j = 15; j >= 0; j--) {
            printf("%d", (opcode >> j) & 1);
        }

        printf(" Hex: [0x%04X] 0x%04X\n", pc, opcode);*/

        pc += 2;

        switch (opcode) {
            case 0x00E0:
                clear_gfx(gfx);
                break;
            case 0x00EE:
                sp--;
                pc = stack[sp];
                break;
        }

        uint8_t first_nibble = opcode >> 12;
        switch (first_nibble) {
            case 0x1:
                // jump to NNN
                pc = opcode & 0x0FFF;
                break;
            case 0x6:
                // set register X to NN

                // pega o X -> (opcode & 0x0F00) >> 8
                // pega o NN -> (opcode & 0x00FF)
                registers[((opcode & 0x0F00) >> 8)] = (opcode & 0x00FF);
                break;
            case 0x7:
                // add register X, NN

                // pega o X -> (opcode & 0x0F00) >> 8
                // pega o NN -> (opcode & 0x00FF)
                registers[((opcode & 0x0F00) >> 8)] += (opcode & 0x00FF);
                break;
            case 0xA:
                // set index
                index_register = opcode & 0x0FFF;
                break;
            case 0xD:
                // display
                draw_gfx(
                    gfx,
                    registers[(opcode & 0xF00) >> 8],
                    registers[(opcode & 0xF0) >> 4], 
                    (opcode & 0xF), 
                    &memory[index_register]
                );
                break;
            default:
                printf("não implementado.");
                break;
        }

        reset_cursor();
        print_gfx(gfx);

        // não passa do tamanho da rom, teoricamente o máximo
        if (pc >= START_ADDRESS + size) {
            break;
        }
    }
    
    return 0;
}