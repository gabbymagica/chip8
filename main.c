#include "beep.h"
#include "gfx.h"
#include "input.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <windows.h>

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50
#define INSTRUCTIONS_PER_SECOND 700

void reset_cursor() {
    printf("\033[H\033[J");
}

int main() {
    FILE *file = fopen("roms/Pong.ch8", "rb");

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

    // troca o terminal pra não canônico
    enable_raw_mode();

    // inicializa o audio
    if (audio_init() != 0) {
        printf("erro inicando o audio");
        return -1;
    }

    // chip-8 variables
    uint8_t memory[4096] = {0};
    uint16_t stack[16] = {0};
    uint8_t sp = 0;
    uint8_t registers[16] = {0};
    uint16_t pc = START_ADDRESS; 
    uint8_t gfx[64 * 32] = {0};
    uint16_t index_register = 0;

    // timers
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;

    // random number generator
    srand(time(NULL));

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

        /*
        printf("DEBUG: OPCODE -- Binário: ");
        for (int j = 15; j >= 0; j--) {
            printf("%d", (opcode >> j) & 1);
        }

        printf(" Hex: [0x%04X] 0x%04X\n", pc, opcode);
        */

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
            case 0x2:
                // calls subroutine at NNN
                stack[sp] = pc;
                sp++;
                pc = opcode & 0x0FFF;
                break;

            // conditionals
            case 0x3:
                // skip next instruction if register X is equal to NN

                // pega o X -> (opcode & 0x0F00) >> 8
                // pega o NN -> (opcode & 0x00FF)
                // incrementa o pc em 2
                if (registers[((opcode & 0x0F00) >> 8)] == (opcode & 0x00FF)) pc += 2;
                break;
            case 0x4:
                // skip next instruction if register X is NOT equal to NN

                // pega o X -> (opcode & 0x0F00) >> 8
                // pega o NN -> (opcode & 0x00FF)
                // incrementa o pc em 2
                if (registers[((opcode & 0x0F00) >> 8)] != (opcode & 0x00FF)) pc += 2;
                break;
            case 0x5:
                // skip next instruction if register X is equal to Y

                // pega o X -> (opcode & 0x0F00) >> 8
                // pega o Y -> (opcode & 0x00F0) >> 4
                // incrementa o pc em 2
                if (registers[((opcode & 0x0F00) >> 8)] == registers[((opcode & 0x00F0) >> 4)]) pc += 2;
                break;
            case 0x9:
                // skip next instruction if register X is NOT equal to Y

                // pega o X -> (opcode & 0x0F00) >> 8
                // pega o Y -> (opcode & 0x00F0) >> 4
                // incrementa o pc em 2
                if (registers[((opcode & 0x0F00) >> 8)] != registers[((opcode & 0x00F0) >> 4)]) pc += 2;
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
            case 0x8:
                // switch last nibble
                switch ((opcode & 0x000F)) {
                    case 0x0:
                        // VX is set to the value of VY

                        // pega o X -> (opcode & 0x0F00) >> 8
                        // pega o Y -> (opcode & 0x00F0) >> 4
                        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x1:
                        // VX is set to the OR of VX and VY. VY is not affected.

                        // pega o X -> (opcode & 0x0F00) >> 8
                        // pega o Y -> (opcode & 0x00F0) >> 4
                        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] | registers[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x2:
                        // VX is set to the AND of VX and VY. VY is not affected.
                        // pega o X -> (opcode & 0x0F00) >> 8
                        // pega o Y -> (opcode & 0x00F0) >> 4
                        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] & registers[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x3:
                        // VX is set to the XOR of VX and VY. VY is not affected.
                        // pega o X -> (opcode & 0x0F00) >> 8
                        // pega o Y -> (opcode & 0x00F0) >> 4
                        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] ^ registers[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x4: 
                        {
                            // VX is set to the SUM of VX and VY. VY is not affected.
                            // pega o X -> (opcode & 0x0F00) >> 8
                            // pega o Y -> (opcode & 0x00F0) >> 4
                            uint8_t x = (opcode & 0x0F00) >> 8;
                            uint8_t y = (opcode & 0x00F0) >> 4;
                            uint16_t soma = registers[x] + registers[y];
                            if (soma > 255) { registers[15] = 1; } else { registers[15] = 0; }
                            registers[x] = registers[x] + registers[y];
                        }
                        break;
                    case 0x5: 
                        {
                            // sets VX to the result of VX - VY
                            uint8_t x = (opcode & 0x0F00) >> 8;
                            uint8_t y = (opcode & 0x00F0) >> 4;
                            if (registers[x] >= registers[y]) { registers[15] = 1; } else { registers[15] = 0; }
                            registers[x] = registers[x] - registers[y];
                        }
                        break;
                    case 0x7: 
                        {
                            // sets VX to the result of VY - VX
                            uint8_t x = (opcode & 0x0F00) >> 8;
                            uint8_t y = (opcode & 0x00F0) >> 4;
                            if (registers[y] >= registers[x]) { registers[15] = 1; } else { registers[15] = 0; }
                            registers[x] = registers[y] - registers[x];
                        }
                        break;
                    case 0x6:
                        {
                            // Shift the value of VX one bit to the right
                            // Set VF to 1 if the bit that was shifted out was 1, or 0 if it was 0
                            uint8_t x = (opcode & 0x0F00) >> 8;
                            registers[15] = registers[x] & 0x1;
                            registers[x] = registers[x] >> 1;
                        }
                        break;
                    case 0xE:
                        {
                            // Shift the value of VX one bit to the left
                            // Set VF to 1 if the bit that was shifted out was 1, or 0 if it was 0
                            uint8_t x = (opcode & 0x0F00) >> 8;
                            // pega o primeiro dígito (já que ele sempre tem 8bits, só fazer um and com 1000 0000)
                            registers[15] = (registers[x] & 0x80) >> 7;
                            registers[x] = registers[x] << 1;
                        }
                        break;
                }
                break;
                
            case 0xA:
                // set index
                index_register = opcode & 0x0FFF;
                break;
            case 0xB:
                // jump with offset of V0
                pc = (opcode & 0x0FFF) + registers[0];
                break;
            case 0xC:
                {
                    // random number generator
                    // This instruction generates a random number, binary ANDs it with the value NN, and puts the result in VX.
                    // pega o X -> (opcode & 0x0F00) >> 8
                    // pega o NN -> (opcode & 0x00FF)

                    uint8_t rng = rand() % 256;
                    registers[(opcode & 0x0F00) >> 8] = rng & (opcode & 0x00FF);
                }
                break;
            case 0xD:
                // display
                registers[15] = draw_gfx(
                    gfx,
                    registers[(opcode & 0xF00) >> 8],
                    registers[(opcode & 0xF0) >> 4], 
                    (opcode & 0xF), 
                    &memory[index_register]
                );
                break;
            case 0xE:
                switch ((opcode & 0x000F)) {
                    case 0xE:
                        if (registers[(opcode & 0x0F00) >> 8] == get_input()) {
                            pc += 2;
                        }
                        break;
                    case 0x1:
                        if (registers[(opcode & 0x0F00) >> 8] != get_input()) {
                            pc += 2;
                        }
                        break;
                }
                break;
            case 0xF:
                switch ((opcode & 0x000F)) {
                    case 0x7:
                        registers[(opcode & 0x0F00) >> 8] = delay_timer;
                        break;                
                    case 0x8:
                        sound_timer = (opcode & 0x0F00) >> 8;
                        break;
                    case 0xE:
                        index_register += (opcode & 0x0F00) >> 8;
                        break;
                    case 0xA:
                        {
                            char ch = get_input();
                            if (ch == -1) {
                                pc -= 2;
                                break;
                            }
                            registers[(opcode & 0x0F00) >> 8] = ch;
                        }
                        break;
                    case 0x9:
                        index_register = memory[FONTSET_START_ADDRESS + (((opcode & 0x0F00) >> 8) * 5)];
                        break;
                    case 0x3:
                        {
                            uint8_t value = registers[(opcode & 0x0F00) >> 8];
                            memory[index_register] = value / 100;
                            memory[index_register + 1] = (value / 10) % 10;
                            memory[index_register + 2] = value % 10;
                        }
                        break;  
                    default:
                        switch ((opcode & 0x00FF)) {
                            case 0x15:
                                // FX15
                                delay_timer = registers[(opcode & 0x0F00) >> 8];
                                break;
                            case 0x55:
                                // FX55
                                for (uint8_t i = 0; i <= (opcode & 0x0F00) >> 8; i++) {
                                    memory[index_register + i] = registers[i];
                                }
                                break;
                            case 0x65:
                                // FX65
                                for (uint8_t i = 0; i <= (opcode & 0x0F00) >> 8; i++) {
                                    registers[i] = memory[index_register + i];
                                }
                                break;
                        }
                        break;
                }
                break;
            default:
                //printf("não implementado.\n");
                break;
        }

       

        if (delay_timer > 0)
        {
            --delay_timer;
        }

        if (sound_timer > 0)
        {
            audio_set_beep(true);
            --sound_timer;
        }

        // 4128 + '\0'
        char print_buffer[4129] = {0};
        gfx_to_print_buffer(gfx, print_buffer);

        reset_cursor();
        printf("%s", print_buffer); 

        // linux: 
        // usleep(16660);
        // windows:
        Sleep(1);

        // não passa do tamanho da rom, teoricamente o máximo
        if (pc >= START_ADDRESS + size) {
            break;
        }
    }
    
    disable_raw_mode();
    return 0;
}