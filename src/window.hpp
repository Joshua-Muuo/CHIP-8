#pragma once
#include "chip8.hpp"
#include <string>
#include <SFML/Graphics.hpp>

class Window
{
public:
    struct Config
    {
        std::optional<u32> width{}, height{};
        std::string title{};
    };
    
public:
    Window(const Config& config);

    bool isOpen();
    void pollEvents();
    void draw();
private:
    void drawDisplay();
private:
    static constexpr u32 PIXEL_SIZE{10u};
    static constexpr u32 WINDOW_WIDTH{CHIP8::COLS * PIXEL_SIZE},
                         WINDOW_HEIGHT{CHIP8::ROWS * PIXEL_SIZE};
private:
    sf::RenderWindow mWindow;
    CHIP8 mCHIP8;
};