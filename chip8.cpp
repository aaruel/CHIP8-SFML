//
//  chip8.cpp
//  SFMLTRY
//
//  Created by Aaron Ruel on 1/14/16.
//  Copyright (c) 2016 AAR. All rights reserved.
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
    
    this->drawScreen = true;
    
    // Loading font set into memory
    for(int i=0; i < FNT_SIZE; ++i){
        this->memory[i] = this->fontset[i];
    }
    
    // Reset hexset
    for(int i=0; i < 16; ++i){
        this->hexkey[i] = false;
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
    
    switch(this->opcode & 0xF000){
        case 0x0000:
            switch(this->opcode & 0x00FF){
                case 0x00E0:
                    // OPCODE: 0x00E0
                    // Clear Screen
                    for (int i=0; i<SCR_SIZE; ++i){
                        this->graphics[i] = 0;
                    }
                    this->programcounter+=2; // PC will be incremented by 2 if processed
                break;
                case 0x00EE:
                    // OPCODE: 0x00EE
                    // Return PC to the top of stack after subroutine
                    --this->stackpointer;
                    this->programcounter = this->stack[this->stackpointer];
                    this->programcounter+=2;
                break;
                default:
                    printf("UNKNOWN OPCODE: %c\n", this->opcode);
                break;
            }
        break;
        case 0x1000:
            // OPCODE: 0x1nnn
            // Set program counter to 'nnn'
            this->programcounter = this->opcode & 0x0FFF;
        break;
        case 0x2000:
            // OPCODE: 0x2nnn
            // Increment stack pointer, puts PC at top of stack, PC set to 'nnn'
            this->stack[this->stackpointer] = this->programcounter;
            ++this->stackpointer;
            this->programcounter = this->opcode & 0x0FFF;
        break;
        case 0x3000:
            // OPCODE: 0x3xkk
            // Compares register Vx to kk, if they are equal the PC is incremented by 2
            if(this->V[(this->opcode & 0x0F00) >> 8] == (this->opcode & 0x00FF)){
                this->programcounter += 4;
            } else {
                this->programcounter += 2;
            }
        break;
        case 0x4000:
            // OPCODE: 0x4xkk
            // Compares register Vx to kk, if they are not equal the PC is incremented by 2
            if(this->V[(this->opcode & 0x0F00) >> 8] != (this->opcode & 0x00FF)){
                this->programcounter += 4;
            } else {
                this->programcounter += 2;
            }
        break;
        case 0x5000:
            // OPCODE: 0x5xy0
            // Compared register Vx and Vy, if equal, PC is incremented by 2
            if(this->V[(this->opcode & 0x0F00) >> 8] == this->V[(this->opcode & 0x00F0) >> 8]){
                this->programcounter += 4;
            } else {
                this->programcounter += 2;
            }
        break;
        case 0x6000:
            // OPCODE: 0x6xkk
            // put value kk into register Vx
            this->V[(this->opcode & 0x0F00) >> 8] = this->opcode & 0x00FF;
            this->programcounter += 2;
        break;
        case 0x7000:
            // OPCODE: 0x7xkk
            // Add the values kk and Vx and stores in Vx
            this->V[(this->opcode & 0x0F00) >>8] = this->V[(this->opcode & 0x0F00) >>8] + (this->opcode & 0x00FF);
            this->programcounter += 2;
        break;
        case 0x8000:
            switch (this->opcode & 0x000F) {
                case 0x0000:
                    // OPCODE: 0x8xy0
                    // Stores value of Vy into Vx
                    this->V[(this->opcode & 0x0F00) >>8] = this->V[(this->opcode & 0x00F0) >>4];
                    this->programcounter += 2;
                break;
                case 0x0001:
                    // OPCODE: 0x8xy1
                    // ORs Vx and Vy and stores result in Vx
                    this->V[(this->opcode & 0x0F00) >>8] |= (this->V[(this->opcode & 0x00F0) >>4]);
                break;
                case 0x0002:
                    // OPCODE: 0x8xy2
                    // ANDs Vx and Vy and stores result in Vx
                    this->V[(this->opcode & 0x0F00) >>8] &= (this->V[(this->opcode & 0x00F0) >>4]);
                    this->programcounter += 2;
                break;
                case 0x0003:
                    // OPCODE: 0x8xy3
                    // XORs Vx and Vy and stores result in Vx
                    this->V[(this->opcode & 0x0F00) >>8] ^= (this->V[(this->opcode & 0x00F0) >>4]);
                    this->programcounter += 2;
                break;
                case 0x0004:
                    // OPCODE: 0x8xy4
                    // ADDs Vx and Vy and stores result in Vx, also adds a flag if the addition carries
                    // The if loop predicts carry
                    if(this->V[(this->opcode & 0x00F0) >>4] > (0xFF - this->V[(this->opcode & 0x0F00) >>8])){
                        this->V[0xF] = 1;
                    } else {
                        this->V[0xF] = 0;
                    }
                    this->V[(this->opcode & 0x0F00) >>8] += (this->V[(this->opcode & 0x00F0) >>4]);
                    this->programcounter += 2;
                break;
                case 0x0005:
                    // OPCODE: 0x8xy5
                    // SUBs Vx and Vy and stores result in Vx, also adds a flag if the subtraction borrows
                    // The if loop predicts borrow
                    if(this->V[(this->opcode & 0x00F0) >>4] > (this->V[(this->opcode & 0x0F00) >>8])){
                        this->V[0xF] = 0;
                    } else {
                        this->V[0xF] = 1;
                    }
                    this->V[(this->opcode & 0x0F00) >>8] -= (this->V[(this->opcode & 0x00F0) >>4]);
                    this->programcounter += 2;
                break;
                case 0x0006:
                    // OPCODE: 0x8xy6
                    // SHR Vx 1 place and stores result in Vx, also adds a flag if LSB is 1
                    // The if loop predicts borrow
                    this->V[0xF] = this->V[(this->opcode & 0x0F00) >>8] & 0x1;
                    this->V[this->opcode & 0x0F00 >>8] >>= 1;
                    this->programcounter += 2;
                break;
                case 0x0007:
                    // OPCODE: 0x8xy7
                    // SUBN Vy and Vx and stores result in Vx, also adds a flag if the subtraction borrows
                    // The if loop predicts borrow
                    if(this->V[(this->opcode & 0x00F0) >>4] < (this->V[(this->opcode & 0x0F00) >>8])){
                        this->V[0xF] = 0;
                    } else {
                        this->V[0xF] = 1;
                    }
                    this->V[(this->opcode & 0x0F00) >>8] = (this->V[(this->opcode & 0x00F0) >>4]) - (this->V[(this->opcode & 0x0F00) >>8]);
                    this->programcounter += 2;
                break;
                case 0x000E:
                    // OPCODE: 0x8xyE
                    // SHL Vx 1 place and stores result in Vx, also adds a flag if MSB is 1
                    // The if loop predicts borrow
                    this->V[0xF] = this->V[(this->opcode & 0x0F00) >>8] >> 7;
                    this->V[(this->opcode & 0x0F00) >>8] <<= 1;
                    this->programcounter += 2;
                break;
                default:
                    printf("UNKNOWN OPCODE: %c\n", this->opcode);
                break;
            }
        break;
        case 0x9000:
            // OPCODE: 0x9xy0
            // If Vx and Vy are not equal, increment PC by 2
            if(this->V[(this->opcode & 0x0F00)] != this->V[this->opcode & 0x00F0]){
                this->programcounter += 4;
            } else {
                this->programcounter += 2;
            }
        break;
        case 0xA000:
            // OPCODE: 0xAnnn
            // Set register I equal to nnn
            this->idex = (this->opcode & 0x0FFF);
            this->programcounter += 2;
        break;
        case 0xB000:
            // OPCODE: 0xBnnn
            // Set PC equal to nnn plus register V0
            this->programcounter = (this->opcode & 0x0FFF) + this->V[0x0];
        break;
        case 0xC000:
            // OPCODE: 0xCxnn
            // Set Vx equal to random number below nn
            this->V[(this->opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (this->opcode & 0x00FF);
            this->programcounter += 2;
        break;
        case 0xD000:{
            // OPCODE: 0xDxyn
            // draw function
            twobytes x = this->V[(this->opcode & 0x0F00) >>8];
            twobytes y = this->V[(this->opcode & 0x00F0) >>4];
            twobytes height = this->opcode & 0x000F;
            this->V[0xF] = 0;
            
            twobytes pixel;
            for(byte yline = 0; yline <height; ++yline){
                pixel = this->memory[this->idex + yline];
                for(byte xline = 0; xline < 8; ++xline){
                    if((pixel & (0x80 >> xline)) != 0){
                        if(this->graphics[(x+xline+((y+yline) * 64))] == 1){
                            V[0xF] = 1; //check if sprite is already in graphics
                        }
                        
                        this->graphics[x + xline + ((y + yline) * 64)] ^= 1; // XOR to screen
                    }
                }
            }
            
            this->drawScreen = true;
            
            this->programcounter += 2;
        break;}
        case 0xE000:
            switch(opcode & 0x00FF){
                case 0x009E:
                    // OPCODE: 0xEx9e
                    // Skip next opcode if key is pressed
                    if(this->hexkey[this->V[(this->opcode & 0x0F00) >>8]]){
                        this->programcounter += 4;
                    }else{
                        this->programcounter += 2;
                    }
                break;
                case 0x00A1:
                    // OPCODE: 0xExa1
                    // Skip next opcode if key is not pressed
                    if(!this->hexkey[this->V[(this->opcode & 0x0F00) >>8]]){
                        this->programcounter += 4;
                    }else{
                        this->programcounter += 2;
                    }
                break;
                default:
                    printf("UNKNOWN OPCODE: %c\n", this->opcode);
                break;
        }
        break;
        case 0xF000:
            switch(this->opcode & 0x00FF){
                case 0x0007:
                    // OPCODE: 0xFx07
                    // Set Vx equal to delay timer
                    this->V[(this->opcode & 0x0F00) >>8] = this->delay_timer;
                    this->programcounter += 2;
                break;
                case 0x000A:{
                    // OPCODE: 0xFx0A
                    // Stop execution unless key is pressed, then store key value in Vx
                    bool keyPress = false;
                    for(int i=0; i<16; ++i){
                        if(hexkey[i]){
                            keyPress = true;
                            this->V[(this->opcode & 0x0F00) >>8] = i;
                            break;
                        }
                    }
                    if(!keyPress){
                        return;
                    }
                    
                    this->programcounter += 2;
                break;}
                case 0x0015:
                    // OPCODE: 0xFx15
                    // Delay timer is set to the value of Vx
                    this->delay_timer = this->V[(this->opcode & 0x0F00) >>8];
                    this->programcounter += 2;
                break;
                case 0x0018:
                    // OPCODE: 0xFx18
                    // Sound timer is set to the value of Vx
                    this->sound_timer = this->V[(this->opcode & 0x0F00) >>8];
                    this->programcounter += 2;
                break;
                case 0x001E:
                    // OPCODE: 0xFx1E
                    // Sound timer is set to the value of Vx
                    this->idex += this->V[(this->opcode & 0x0F00) >>8];
                    this->programcounter += 2;
                break;
                case 0x0029:
                    // OPCODE: 0xFx29
                    // idex is set to location of hexadecimal sprite corresponding to Vx (relating to font set)
                    this->idex = this->V[(this->opcode & 0x0F00) >>8] * 0x5;
                    this->programcounter += 2;
                break;
                case 0x0033:
                    // OPCODE: 0xFx33
                    // Take decimal value of Vx and store in memory location idex incrementing with place digits
                    this->memory[this->idex] = this->V[(this->opcode & 0x0F00 >>8)]/100;
                    this->memory[this->idex + 1] = (this->V[(this->opcode & 0x0F00 >>8)]/10)%10;
                    this->memory[this->idex + 1] = (this->V[(this->opcode & 0x0F00 >>8)]%10)%10;
                    this->programcounter += 2;
                break;
                case 0x0055:
                    // OPCODE: 0xFx55
                    // Store registers V0 through Vx into memory starting at location idex
                    for(int i=0; i<((this->opcode & 0x0F00) >>8); ++i){
                        this->memory[this->idex + i] = this->V[0x0 + i];
                    }
                    // Increment idex so it's pointing outside of the newly inserted values
                    idex += ((this->opcode & 0x0F00) >> 8) + 1;
                    
                    this->programcounter += 2;
                break;
                case 0x0065:
                    // OPCODE: 0xFx65
                    // Store memory into registers V0 through Vx starting at location idex
                    for(int i=0; i<((this->opcode & 0x0F00) >>8); ++i){
                        this->V[0x0 + i] = this->memory[this->idex + i];
                    }
                    // Increment idex so it's pointing outside of the newly inserted values
                    idex += ((this->opcode & 0x0F00) >> 8) + 1;
                    
                    this->programcounter += 2;
                break;
                default:
                    printf("UNKNOWN OPCODE: %c\n", this->opcode);
                break;
            }
        break;
        default:
            printf("UNKNOWN OPCODE: %c\n", this->opcode);
        break;
    }
    if(this->delay_timer > 0) --this->delay_timer;
    if(this->sound_timer > 0) --this->sound_timer;
}

unsigned char *chip::getGraphics(){
    return this->graphics;
}

bool chip::drawScreenF(){
    return this->drawScreen;
}

bool chip::setScreenF(bool set){
    this->drawScreen = set;
}

void chip::setKeyState(unsigned int key, bool state){
    this->hexkey[key] = state;
}
