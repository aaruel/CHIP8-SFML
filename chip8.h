//
//  chip8.h
//

#ifndef __SFMLTRY__chip8__
#define __SFMLTRY__chip8__
#pragma once
#include <stdio.h>

class chip {
    typedef unsigned char byte;
    typedef unsigned short twobytes;
    
    static const int MEM_SIZE = 4096;
    static const int REG_SIZE = 16;
    static const int SCR_SIZE = 64*32;
    static const int STK_SIZE = 16;
    static const int HEX_SIZE = 16;
    static const int FNT_SIZE = 80;
    
    public:
    
        void initialize();
        void emulateCycle();
        bool loadGame();
        byte *getGraphics();
        bool drawScreenF();
        bool setScreenF(bool set);
        void setKeyState(unsigned int key, bool state);
    
        // two byte opcodes
        twobytes opcode;
        
        // 4096 bytes of memory
        byte memory[MEM_SIZE];
        
        // 16 V registers (8-bit)
        byte V[REG_SIZE];
        
        // 12-bit (rounded to 16-bit) index and program counter
        // 0x000 to 0xFFF
        twobytes idex;
        twobytes programcounter;
        
        // memory partitioning
        // 0x000 to 0x1FF -- Interpreter
        // 0x050 to 0x0A0 -- Font Set
        // 0x200 to 0xFFF -- ROM & RAM
        
        // 2048 pixel screen (64x32 resolution)
        byte graphics[SCR_SIZE];
        
        // timer registers for sound
        // if sound_timer == 0 then buzzer sounds
        // timers constantly counting down at 60hz
        byte delay_timer;
        byte sound_timer;
        
        // 16 level of stack need to be implemented
        // along with a pointer
        twobytes stack[STK_SIZE];
        byte  stackpointer;
        
        // Array to store 16 hex codes
        byte hexkey[HEX_SIZE];
        
        // Defining the fontset
        byte fontset[FNT_SIZE] =
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
        // Draw screen flag
        bool drawScreen;
};

#endif /* defined(__SFMLTRY__chip8__) */
