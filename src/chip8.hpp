#pragma once
#include <array>
#include "types.hpp"
#include <string>
#include <chrono>

#define KB 1024

using timepoint = std::chrono::time_point<std::chrono::system_clock>;
class CHIP8
{
public:
    static constexpr u32 ROWS{32}, COLS{64};
public:
    CHIP8();
    void loadROM(const std::string& romFile);
	
    const std::array<bool, ROWS * COLS>& getDisplay();
	void setKey(u8 keyValue, bool on = true);

    void run();
private:
    static constexpr bool BLACK{0}, WHITE{1};
    static constexpr u32 FONT_LOCATION{0x050};
    static constexpr u32 PC_INITIAL{0x200};
    static constexpr u32 INSTRUCTIONS_PER_CALL{10};
	static constexpr f32 FPS_MS{1000.0f / 60};

	enum class Registers
    {
        V0, V1, V2, V3, V4, V5, V6, V7, 
        V8, V9, VA, VB, VC, VD, VE, VF
    };
private:
    void initRAM();
    void fetch();
    void execute();
	u8& getRegister(const Registers& reg);
	const u8& getRegister(const Registers& reg) const;
private: 
    std::array<u8, 4 * KB> mMemory{};
    std::array<bool, ROWS * COLS> mDisplay{};
    std::array<bool, 16> mKeyMap{};
    std::array<u8, 16> mRegisters{};
    std::array<u16, 16> mStack{};
    u16 mPC{}, mI{};
    u8 mDelayTimer{60}, mSoundTimer{60};
    u8 mSP{};
    u16 mCurrentOpCode{};

	u8 mKeyPressed;
	bool mWaitingForKeyPress{false};
	f32 mDeltaTime{};
	timepoint mLastCall, mThisCall;
};
