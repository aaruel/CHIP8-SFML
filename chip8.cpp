//
//  chip8.cpp
//


#include "chip8.h"
#include <stdlib.h>

void chip::initialize() {
    // setting initial values on start
    this->programcounter = 0x200;
    this->opcode         = 0;
    this->idex           = 0;
    this->stackpointer   = 0;
    
    // clearing init values to ensure proper use
    for(int i=0; i < SCR_SIZE; ++i){
        this->graphics[i] = 0;
    }
    
    for(int i=0; i < STK_SIZE; ++i){
        this->stack[i] = 0;
    }
    
    for(int i=0; i < REG_SIZE; ++i){
        this->V[i] = 0;
    }
    
    for(int i=0; i < MEM_SIZE; ++i){
        this->memory[i] = 0;
    }
    
    // Loading font set into memory
    for(int i=0; i < FNT_SIZE; ++i){
        this->memory[i] = fontset[i];
    }
    
    // (Re)setting timers at zero
    this->sound_timer = 0;
    this->delay_timer = 0;
}

bool chip::loadGame(){
    FILE *f = fopen("breakout.ch8", "rb");
    if (f==NULL){
        return false;
    } else {
        printf("Game loaded!\n");
    }
    // Get filesize before loading into memory
    fseek(f, 0L, SEEK_END);
    int filesize = ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    // memory allocate game code into buffer
    unsigned char *buffer=(unsigned char*)malloc(filesize);
    fread(buffer, 1, filesize, f);
    
    // load buffer into memory
    for(int i=0; i < filesize; ++i){
        this->memory[i + 512] = buffer[i];
    }
    
    fclose(f);
    
    return true;
}

void chip::emulateCycle() {
    // Stage One: Fetch opcode
    // Program Counter : A register which holds the address of next instruction
    // when the current is under execution
    // Stack Pointer   : A pointer which points to the top element of the stack
    // This loads the opcode by getting the first 8 bits (1st byte) into a 16bit
    // char (ex. byte a2 would be 0000000010100010) then it shifts left 8 bits
    // (1010001000000000) then ORs the next two bits (byte cc OR compared would be
    // 1010001011001100)
    opcode = this->memory[this->programcounter] << 8 | this->memory[this->programcounter+1];
    
    // The AND bit operator allows the first byte to be pass through to the case statements
    // because the limiter being compared(example : |1010|001000000000 & |1111|000000000000)
    
    switch(opcode & 0xF000){
            
    }
}
