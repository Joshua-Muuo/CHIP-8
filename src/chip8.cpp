#include "chip8.hpp"
#include <algorithm>
#include <fstream>
#include <print>
#include <filesystem>
#include <random>

namespace 
{
	u8 randByte()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());

		static std::uniform_int_distribution<u8> distrib(0, 255);
		return distrib(gen);
	}
};
CHIP8::CHIP8()
{
    initRAM();
    loadROM("roms/snake.ch8");
}

void CHIP8::initRAM()
{
    mPC = PC_INITIAL;
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

void CHIP8::loadROM(const std::string& romFile)
{
    std::filesystem::path romPath{romFile};
    if (!std::filesystem::exists(romPath))
    {
        std::println(stderr, "Error: rom could not be found");
        return;
    }

    const usize INSTRUCTIONS_SIZE{mMemory.size() - PC_INITIAL};

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
	if (!rom)
	{
        std::println(stderr, "Error: rom could not be read");
        return;
	}
    rom.read(reinterpret_cast<char*>(mMemory.data()) + PC_INITIAL, romFileSize);

    for (u32 i{0}; i < 0x20; ++i)
    {
        std::println("{:X}", mMemory[i + PC_INITIAL]);
    }
}

const std::array<bool, CHIP8::ROWS * CHIP8::COLS>& CHIP8::getDisplay()
{
    return mDisplay;
}


void CHIP8::setKey(u8 keyValue, bool on)
{
	if (keyValue > 0xF) return;
	mKeyMap[keyValue] = on;

	if (on && mWaitingForKeyPress)
	{
		mWaitingForKeyPress = false;
		mKeyPressed = keyValue;
		std::println("done waiting");
	}

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
       x            {static_cast<u8>(firstByte & 0xF)},
       y            {static_cast<u8>(KK >> 4)},
       fourthNibble {static_cast<u8>(KK & 0xF)};
   
    // nnn represents the second, third, and fourth (last three) nibbles in the op code 
    // shift the opcode to the left by 4 bits to get rid of the first nibble, 
    // then undo the shift
    u16 nnn{static_cast<u16>(mCurrentOpCode & 0xFFF)};

    // std::println("instruction code is {:04X}\nfirstByte is {:02X}, KK is {:02X}, nnn is {:03X}", mCurrentOpCode, firstByte, KK, nnn);
    // std::println("PC is {:X}", mPC);
    // if (count > 5)
    //     std::exit(0);
	
	u8& Vx{mRegisters[x]}, &Vy{mRegisters[y]};
    switch (firstNibble)
    {
        case 0:
        {
            // instruction 0nnn will not be implemented

            if (KK == 0xE0)
                mDisplay.fill(BLACK);
			if (KK == 0xEE)
			{
				mPC = mStack[mSP - 1];
				--mSP;
			}
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
            mStack[mSP++] = mPC;
            mPC = nnn;
            break;
        }

        case 3:
        {
            // 3xkk skips the next instruction if Vx = kk
            if (mRegisters[x] == KK)
                mPC += 2;

            break;
        }

        case 4:
        {
            // 4xkk skips the next instruction if Vx != kk
            if (mRegisters[x] != KK)
                mPC += 2;
            
            break;
        }

        case 5:
        {
            // 5xy0 skips the next instruction if Vx == Vy

			if (fourthNibble != 0) break;
            if (mRegisters[x] == mRegisters[y])
                mPC += 2;

            break;
        }
        
        case 6:
        {
            // 6xkk loads kk into Vx (Vx = kk)
            Vx = KK;
            break;
        }

        case 7:
        {
            // 7xKK adds byte KK into register Vx
            Vx += KK;
            std::println("added {} to register {}", KK, x);
            break;
        }

		case 8:
		{
			switch (fourthNibble)
			{
				case 0:
					// 8xy0 loads Vy into Vx
					Vx = Vy;
					break;
				case 1:
					// 8xy1 loads the bitwise AND of Vx and Vy into Vx
					Vx = Vx & Vy;
					break;
				case 2:
					// 8xy2 loads the bitwise OR of Vx and Vy into Vx
					Vx = Vx | Vy;
					break;
				case 3:
					// 8xy3 loads the bitwise XOR of Vx and Vy into Vx
					Vx = Vx ^ Vy;
					break;
				case 4:
				{
					// 8xy4 loads the sum of Vx and Vy into Vx
					// If the sums overflows, VF is set to 1, else 0
					u16 sum = static_cast<u16>(Vx) + Vy;
					bool overflow{sum > 255};
					if (overflow)
						sum -= 255;
					
					Vx = sum;
					getRegister(Registers::VF) = overflow;
					break;
				}
				case 5:
				{
					// 8xy5 loads the difference of Vx and Vy into Vx
					// If the result doesn't underflow (Vx > Vy, NOT underflow), then
					// VF is set to 1, else 0
					
					bool underflow{Vx > Vy};
					Vx -= Vy;
					getRegister(Registers::VF) = !underflow;
					break;
				}
				case 6:
				{
					// 8xy6 bitwise shifts the value stored in Vx to the right by one
					// If the least significant bit in Vx was 1, then VF is set to 1, else 0 
					u8 lsb{static_cast<u8>(Vx & 1)};
					Vx >>= 1;

					getRegister(Registers::VF) = lsb;
					break;
				}
				case 7:
				{
					// 8xy7 loads the difference of Vy and Vx into Vx
					// If the result doesn't underflow (Vy > Vx, NOT underflow), then
					// VF is set to 1, else 0
					bool underflow{Vy > Vx};
					Vx = Vy - Vx;
					getRegister(Registers::VF) = !underflow;
					break;
				}
				case 0xE:
					// 8xy7 bitwise shifts the value stored in Vx to the left by one
					// If the most significant bit in Vx was 1, then VF is set to 1, else 0 
					u8 msb{static_cast<u8>((Vx >> 7) & 1)};
					Vx <<= 1;

					getRegister(Registers::VF) = msb;
					break;
			}
		}

		case 0x9:
		{
			// 9xy0 skips the next instruction if Vx != Vy
			if (fourthNibble != 0) break;
			if (Vx != Vy) mPC += 2;
			break;
		}

        case 0xA:
        {
            // Annn sets the value of the index register to nnn
            mI = nnn;
            break;
        }

        case 0xB:
        {
            // Bnnn jumps to instruction nnn + V0
            mPC = nnn + getRegister(Registers::V0);
            break;
        }
		case 0xC:
		{
            // Cxkk performs a bitwise AND of a random byte and KK, then
			// store the result in Vx
			Vx = randByte() & KK;
			break;
		}
        case 0xD:
        {
            // Dxyn draws an n-byte sprite starting at the memory location stored in the
            // index register at (Vx, Vy)
 

            // Before drawing a sprite, register VF has to be 
            // set to 0. This is because if a filled in sprite is overwritten,
            // register VF is set to 1.
            getRegister(Registers::VF) = 0;

            // The starting position of the sprite is held in registers
            // Vx and Vy. An important thing to note though is that it can wrap around,
            // meaning that because the display has dimensions 64x32, a starting position of
            // (65, 33) would be the same as (1, 1).
            // Only the starting position should wrap around however. If a sprite were to go off 
            // of the display, it should be clipped.
            u16 x{static_cast<u16>(Vx % COLS)},
                y{static_cast<u16>(Vy % ROWS)};

            // The fourth nibble stores the byte size of the sprite
            u8 nBytes{fourthNibble};
            for (i32 n{}; n < nBytes; ++n)
            {
                // Stop drawing the sprite if it goes below the display.
                if (y + n >= ROWS)
                    break;

				if (mI + n >= mMemory.size()) break;
                u8 byte{mMemory[mI + n]};
                for (i32 dx{}; dx < 8; ++dx)
                {
                    // Stop drawing the current row if it goes outside of the display.
                    if (x + dx >= COLS)
                        break;

                    u32 index{(y + n) * COLS + x + dx};
					if (index >= mDisplay.size()) break;
                    u8 bit{static_cast<u8>((byte >> (7 - dx)) & 1)};

                    if (mDisplay[index] && bit)
                        getRegister(Registers::VF) = 1;
                    
                    mDisplay[index] ^= bit;
                }
            }
        }

		case 0xE:
		{
			switch (KK)
			{
				case 0x9E:
				{
					// Ex9E skips next instruction if the key with the value
					// of Vx is currently pressed
					if (mKeyMap[Vx]) 
						mPC += 2;

					break;
				}
				case 0xA1:
				{
					// ExA1 skips next instruction if the key with the value
					// of Vx isn't currently pressed
					if (!mKeyMap[Vx]) 
						mPC += 2;
					break;
				}
			}
		}

		case 0xF:
		{
			switch (KK)
			{
				case 0x07:
					// Fx0A stores the value of the delay timer in Vx
					Vx = mDelayTimer;
					break;
				case 0x0A:
					// Fx0A stops execution until a key is pressed, then stores
					// the value of that key in Vx
					mWaitingForKeyPress = true;		
					mPC -= 2;
					break;
				case 0x15:
					// Fx15 sets the delay timer to Vx
					mDelayTimer = Vx;
					break;
				case 0x18:
					// Fx18 sets the sound timer to Vx
					mSoundTimer = Vx;
					break;
				case 0x1E:
					// Fx1E increments the index register by Vx
					mI += Vx;
					break;
				case 0x29:
					// Fx29 points the index register to the location of 
					// digit Vx
					mI = Vx * 5 + FONT_LOCATION;
					break;

				case 0x33:
					// Fx33 stores the value of BCD (binary coded decimal)
					// representation of Vx in memory addresses 
					// I, I + 1, I + 2
					// The hundreds digit is stored at I,
					// the tens digit at I + 1, and the ones digit 
					// at I + 2
					mMemory[mI] = Vx / 100;
					mMemory[mI + 1] = (Vx / 10) % 10;
					mMemory[mI + 2] = Vx % 10;
					break;
				case 0x55:
					// Fx55 stores registers V0 through Vx in memory,
					// starting at address I
					for (u32 i{}; i <= x; ++i)
					{
						mMemory[mI + i] = mRegisters[i];
					}

					mI +=  x + 1;
					break;
				case 0x65:  
					// Fx65 stores addresses in memory into 
					// registers V0 through Vx, starting at 
					// address I
					for (u32 i{}; i <= x; ++i)
					{
						mRegisters[i] = mMemory[mI + i];
					}

					mI += x + 1;
					break;
			}
		}
    }

    ++count;
} 

void CHIP8::run()
{ 
	if (mInstructionClock.tick() && !mWaitingForKeyPress)
	{
		fetch(); 
		execute(); 
	}

	if (mTimerClock.tick())
	{
		if (mDelayTimer > 0) --mDelayTimer;
		if (mSoundTimer > 0) --mSoundTimer;
	}
}

u8& CHIP8::getRegister(const Registers& reg)
{
	return mRegisters[static_cast<usize>(reg)];
}

const u8& CHIP8::getRegister(const Registers& reg) const
{
	return mRegisters[static_cast<usize>(reg)];
}
