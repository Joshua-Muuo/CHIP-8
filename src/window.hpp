#pragma once
#include "chip8.hpp"
#include <string>
#include <SFML/Graphics.hpp>

class Window
{
public:
    struct Config
    {
        u32 width{100u}, height{100u};
        std::string title{};
    };

public:
    Window(const Config& config);

    bool isOpen();
    void pollEvents();
    void draw();
private:
    sf::RenderWindow mWindow;
    CHIP8 mCHIP8;
};