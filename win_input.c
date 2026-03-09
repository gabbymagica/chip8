#include "input.h"
#include <conio.h>
#include <windows.h>

static DWORD original_console_mode;

void enable_raw_mode() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &original_console_mode);
    DWORD new_mode = original_console_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    SetConsoleMode(hStdin, new_mode);
}

void disable_raw_mode() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hStdin, original_console_mode);
}

char get_input() {
    if (!_kbhit()) {
        return -1;
    }

    int ch = _getch();
    
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