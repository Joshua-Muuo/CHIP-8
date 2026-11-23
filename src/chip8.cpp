#include "chip8.hpp"
#include <algorithm>
#include <fstream>
#include <print>
#include <filesystem>

CHIP8::CHIP8()
{
    initRAM();
    loadROM("roms/ibm-logo.ch8");
}

void CHIP8::initRAM()
{
    mPC = PC_INITIAL;
    std::array<u8, 5 * 16> font{
    // font from  https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
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

void CHIP8::loadROM(const std::string& romFile)
{
    std::filesystem::path romPath{romFile};
    if (!std::filesystem::exists(romPath))
    {
        std::println(stderr, "Error: rom could not be found");
        return;
    }

    const u32 INSTRUCTIONS_SIZE{mMemory.size() - PC_INITIAL};

    std::error_code ec;
    usize romFileSize{std::filesystem::file_size(romPath, ec)};
    if (ec)
    {
        std::println(stderr, "Error: {}", ec.message());
        return;
    }

    if (romFileSize > INSTRUCTIONS_SIZE)
    {
        std::println(stderr, "Error: rom is too big to fit in memory");
        return;
    }

    std::ifstream rom{romPath, std::ios::binary};
    rom.read(reinterpret_cast<char*>(mMemory.data()) + PC_INITIAL, INSTRUCTIONS_SIZE);

    for (u32 i{0}; i < 0x20; ++i)
    {
        std::println("{:X}", mMemory[i + PC_INITIAL]);
    }
}

const std::array<bool, CHIP8::ROWS * CHIP8::COLS>& CHIP8::getDisplay()
{
    return mDisplay;
}

void CHIP8::fetch()
{
    // mCurrentOpCode = mMemory[mPC + 1] << 8 | mMemory[mPC];
    std::println("{:02X} and {:02X}", mMemory[mPC], mMemory[mPC + 1]);
    mCurrentOpCode = mMemory[mPC] << 8 | mMemory[mPC + 1];
    mPC += 2;
}

void CHIP8::execute()
{
    static u32 count{};
    u8 firstByte{static_cast<u8>((mCurrentOpCode >> 8) & 0xFF)},
       KK       {static_cast<u8>(mCurrentOpCode      & 0xFF)};

    u8 firstNibble  {static_cast<u8>(firstByte >> 4)},
       Vx           {static_cast<u8>(firstByte & 0xF)},
       Vy           {static_cast<u8>(KK >> 4)},
       fourthNibble {static_cast<u8>(KK & 0xF)};
   
    // nnn represents the second, third, and fourth (last three) nibbles in the op code 
    // shift the opcode to the left by 4 bits to get rid of the first nibble, 
    // then undo the shift
    u16 nnn{static_cast<u16>(mCurrentOpCode & 0xFFF)};

    std::println("instruction code is {:04X}\nfirstByte is {:02X}, KK is {:02X}, nnn is {:03X}", mCurrentOpCode, firstByte, KK, nnn);
    std::println("PC is {:X}", mPC);
    // if (count > 5)
    //     std::exit(0);
    switch (firstNibble)
    {
        case 0:
        {
            // instruction 0nnn will not be implemented

            if (KK == 0xE0)
                mDisplay.fill(BLACK);
            break;
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
            std::println("added {} to register {}", KK, Vx);
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

            std::println("reached draw instruction");
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

            std::println("starting at ({}, {})", x, y);

            // The fourth nibble stores the byte size of the sprite
            u8 nBytes{fourthNibble};
            std::println("started draw at index {:X}, drawing {} bytes", mI, nBytes);
            for (i32 n{}; n < nBytes; ++n)
            {
                // Stop drawing the sprite if it goes below the display.
                if (y >= ROWS)
                    break;

                u8 byte{mMemory[mI + n]};
                for (i32 dx{}; dx < 8; ++dx)
                {
                    // Stop drawing the current row if it goes outside of the display.
                    if (x + dx >= COLS)
                        break;

                    std::println("n is {}, y is {}", n, y);
                    std::println("x is {}, dx is {}", x, dx);
                    u32 index{(y + n) * COLS + x + dx};
                    u8 bit{static_cast<u8>((byte >> (8 - dx - 1)) & 1)};

                    std::println("draw bit {} at index {}", bit, index);
                    if (mDisplay[index])
                        mRegisters[static_cast<u32>(REGISTERS::VF)] = 1;
                    else
                        std::println("drawing white to screen");
                    
                    mDisplay[index] ^= 1;
                }
            }
        }
    }

    ++count;
} 

void CHIP8::run()
{ 
    fetch(); 
    execute(); 

    if (mDelayTimer > 0) --mDelayTimer;
    if (mSoundTimer > 0) --mSoundTimer;

}