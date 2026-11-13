#include <array>
#include "types.hpp"

#define KB 1024

class CHIP8
{
private:
    static constexpr u32 FONT_LOCATION{0x050};
    static constexpr u32 ROWS{64}, COLS{32};
    static constexpr bool BLACK{0}, WHITE{1};

public:
    CHIP8();
    void loadROM();
    const std::array<bool, ROWS * COLS>& getDisplay();
private:
    void initRAM();
    void fetch();
    void execute();
private:
    enum class REGISTERS
    {

        V0, V1, V2, V3, V4, V5, V6, V7, 
        V8, V9, VA, VB, VC, VD, VE, VF
    };
    
    std::array<u8, 4 * KB> mMemory{};
    std::array<bool, ROWS * COLS> mDisplay{};
    std::array<u8, 16> mRegisters{};
    std::array<u16, 16> mStack{};
    u16 mPC{}, mI{};
    u8 mDelayTimer{}, mSoundTimer{};
    u8 mSP{};
    u16 mCurrentOpCode{};
};