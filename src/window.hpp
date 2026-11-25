#pragma once
#include "chip8.hpp"
#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_map>

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
	std::unordered_map<sf::Keyboard::Key, u8> mKeyMap
	{
		{
			{sf::Keyboard::Key::Num1, 0x1},
			{sf::Keyboard::Key::Num2, 0x2},
			{sf::Keyboard::Key::Num3, 0x3},
			{sf::Keyboard::Key::Num4, 0xC},

			{sf::Keyboard::Key::Q, 0x4},
			{sf::Keyboard::Key::W, 0x5},
			{sf::Keyboard::Key::E, 0x6},
			{sf::Keyboard::Key::R, 0xD},

			{sf::Keyboard::Key::A, 0x7},
			{sf::Keyboard::Key::S, 0x8},
			{sf::Keyboard::Key::D, 0x9},
			{sf::Keyboard::Key::F, 0xE},

			{sf::Keyboard::Key::Z, 0xA},
			{sf::Keyboard::Key::X, 0x0},
			{sf::Keyboard::Key::C, 0xB},
			{sf::Keyboard::Key::V, 0xF},
		}
	};
};
