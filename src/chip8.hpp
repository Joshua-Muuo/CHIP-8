#include <array>
#include "types.hpp"

class CHIP8
{
public:
    CHIP8();
    void loadROM();
private:
    void initRAM();
    void fetch();
    void decode();
    void execute();
private:
    u32 FONT_LOCATION{0x050};
private:
    std::array<u8, 1024 * 4> mMemory{};
    std::array<bool, 64 * 32> mDisplay{};
    u32 mPC{}, mI{}, mDelayTimer{}, mSoundTimer{};
    struct
    {
        u8 V0{}, V1{}, V2{}, V3{}, V4{}, V5{}, V6{}, V6{}, 
        V7{}, V8{}, V9{}, VA{}, VB{}, VC{}, VD{}, VE{}, VF{};
    } mRegisters;
};