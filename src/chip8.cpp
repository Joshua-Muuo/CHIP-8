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

const std::array<bool, CHIP8::ROWS * CHIP8::COLS>& CHIP8::getDisplay()
{
    return mDisplay;
}

void CHIP8::fetch()
{
    mCurrentOpCode = mMemory[mPC + 1] << 8 | mMemory[mPC];
    mPC += 2;
}

void CHIP8::execute()
{
    u8 firstByte{mCurrentOpCode >> 8 && 0xFF},
       KK       {mCurrentOpCode      && 0xFF};

    u8 firstNibble  {static_cast<u8>(firstByte >> 4)},
       Vx           {static_cast<u8>(firstByte & 0xF)},
       Vy           {static_cast<u8>(KK >> 4)},
       fourthNibble {static_cast<u8>(KK & 0xF)};
   
    // nnn represents the second, third, and fourth (last three) nibbles in the op code 
    // shift the opcode to the left by 4 bits to get rid of the first nibble, 
    // then undo the shift
    u16 nnn{static_cast<u16>((mCurrentOpCode << 4) >> 4)};
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
            // Dxyn draws an n-byte sprite starting at the memory location stored in the
            // index register at (Vx, Vy)
 

            // Before drawing a sprite, register VF has to be 
            // set to 0. This is because if a filled in sprite is overwritten,
            // register VF is set to 1.
            mRegisters[static_cast<i32>(REGISTERS::VF)] = 0;

            // The starting position of the sprite is held in registers
            // Vx and Vy. An important thing to note though is that it can wrap around,
            // meaning that because the display has dimensions 64x32, a starting position of
            // (65, 33) would be the same as (1, 1).
            // Only the starting position should wrap around however. If a sprite were to go off 
            // of the display, it should be clipped.
            u16 x{static_cast<u16>(mRegisters[Vx] % COLS)},
                y{static_cast<u16>(mRegisters[Vy] % ROWS)};

            // The fourth nibble stores the byte size of the sprite
            u8 nBytes{fourthNibble};
            for (usize n{}; n < nBytes; ++n)
            {
                // Stop drawing the sprite if it goes below the display.
                if (y > ROWS)
                    break;

                u8 byte{mMemory[mI + n]};
                for (i32 dx{}; dx < 8; ++dx)
                {
                    // Stop drawing the current row if it goes outside of the display.
                    if (x + dx > COLS)
                        break;

                    u32 index{y * ROWS + x + dx};
                    u8 bit{static_cast<u8>(byte >> (8 - dx + 1) & 1)};
                    if (mDisplay[index] == WHITE)
                        mRegisters[static_cast<u32>(REGISTERS::VF)] = 1;
                    
                    mDisplay[index] ^= 1;
                }
                ++y;
            }
        }

    }
}