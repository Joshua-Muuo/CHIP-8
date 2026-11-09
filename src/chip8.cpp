#include "chip8.hpp"
#include <algorithm>

CHIP8::CHIP8()
{
    initRAM();
}

void CHIP8::initRAM()
{
    std::array<u8, 5 * 16> font{
    // font from  https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
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

    std::ranges::copy(font, mMemory.begin() + FONT_LOCATION);
}

void CHIP8::fetch()
{
    mCurrentOpCode = std::byteswap(mMemory[mPC]);
    mPC += 2;
}

void CHIP8::execute()
{
    u8 firstNibble  {(mCurrentOpCode >> 12) & 0x0F},
       Vx {(mCurrentOpCode >> 8)  & 0x0F},
       Vy  {(mCurrentOpCode >> 4)  & 0x0F},
       fourthNibble {mCurrentOpCode         & 0x0F};
    
    u8 firstByte{mCurrentOpCode >> 8},
       KK{mCurrentOpCode && 0x00FF};
    
    u16 nnn{mCurrentOpCode >> 8};
    switch (firstNibble)
    {
        case 0:
        {
            // instruction 0nnn will not be implemented

            if (KK == 0xE0)
                mDisplay.fill(BLACK);
        }

        case 1:
        {
            // 1nnn sets the PC to nnn
            mPC = nnn;
            break; 
        }

        case 2:
        {
            // 2nnn calls the subroutine at nnn
            mStack[++mSP] = mPC;
            mPC = nnn;
            break;
        }

        case 3:
        {
            // 3xkk skips the next instruction if Vx = kk

            if (mRegisters[Vx] == KK)
                mPC += 2;

            break;
        }

        case 4:
        {
            // 4xkk skips the next instruction if Vx != kk
            if (mRegisters[Vx] != KK)
                mPC += 2;
            
            break;
        }

        case 5:
        {
            // 5xy0 skips the next insturction if Vx == Vy
            if (mRegisters[Vx] == mRegisters[Vy])
                mPC += 2;

            break;
        }
        
        case 6:
        {
            // 6xkk loads kk into Vx (Vx = kk)
            mRegisters[Vx] = KK;
            break;
        }

        case 7:
        {
            // 7xKK adds byte KK into register Vx
            mRegisters[Vx] += KK;
            break;
        }

        case 0xA:
        {
            // Annn sets the value of the index register to nnn
            mI = nnn;
            break;
        }

        case 0xD:
        {
            // Dxyn draws n-byte sprite starting at the memory location stored in the
            // index register at (Vx, Vy)
 

            mRegisters[static_cast<i32>(REGISTERS::VF)] = 0;
            u16 x{mRegisters[Vx] % COLS},
                y{mRegisters[Vy] % ROWS};

            u8 nBytes{fourthNibble};
            for (usize n{}; n < nBytes; ++n)
            {
                u8 byte{mMemory[mI + n]};
                for (i32 i{}; i < 8; ++i)
                {
                    u8 bit{byte >> (8 - i) & 0xFF};
                    if (mDisplay[y * ROWS + x] == WHITE)
                        mRegisters[static_cast<i32>(REGISTERS::VF)] = 0;
                }
            }

            drawSprite();
        }

    }
}

void CHIP8::drawSprite()
{   

}