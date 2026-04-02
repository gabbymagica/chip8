#include "input.h"
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void enable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void disable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
} 

void update_input(uint8_t* keypad) {
    int ch = getchar();
    uint8_t* changed = &keypad[16];
    
    switch (ch) {
        case '1': keypad[1] = 4; *changed = 1; break;
        case '2': keypad[2] = 4; *changed = 2; break;
        case '3': keypad[3] = 4; *changed = 3; break;
        case '4': keypad[12] = 4; *changed = 12; break;
        
        case 'q': case 'Q': keypad[4] = 4; *changed = 4; break;
        case 'w': case 'W': keypad[5] = 4; *changed = 5; break;
        case 'e': case 'E': keypad[6] = 4; *changed = 6; break;
        case 'r': case 'R': keypad[13] = 4; *changed = 13; break;
        
        case 'a': case 'A': keypad[7] = 4; *changed = 7; break;
        case 's': case 'S': keypad[8] = 4; *changed = 8; break;
        case 'd': case 'D': keypad[9] = 4; *changed = 9; break;
        case 'f': case 'F': keypad[14] = 4; *changed = 14; break;
        
        case 'z': case 'Z': keypad[10] = 4; *changed = 10; break; 
        case 'x': case 'X': keypad[0] = 4; *changed = 0; break;
        case 'c': case 'C': keypad[11] = 4; *changed = 11; break;
        case 'v': case 'V': keypad[15] = 4; *changed = 15; break;
    }
}