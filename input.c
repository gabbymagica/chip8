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

char get_input() {
    int ch = getchar();
    
    switch (ch) {
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 12; 
        
        case 'q': case 'Q': return 4;
        case 'w': case 'W': return 5;
        case 'e': case 'E': return 6;
        case 'r': case 'R': return 13; 
        
        case 'a': case 'A': return 7; 
        case 's': case 'S': return 8;
        case 'd': case 'D': return 9;
        case 'f': case 'F': return 14;
        
        case 'z': case 'Z': return 10; 
        case 'x': case 'X': return 0;
        case 'c': case 'C': return 11;
        case 'v': case 'V': return 15;
        
        default: return -1;
    }
}