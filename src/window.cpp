#include "window.hpp"
#include <print>

Window::Window(const Config& config)
{
    if (!config.width.has_value() || !config.height.has_value())
    {
        mWindow = sf::RenderWindow(
            sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), 
            config.title
        );
    
        std::println("Creating window of {} x {}", WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    else
    {
        mWindow = sf::RenderWindow(
            sf::VideoMode({config.width.value(), config.height.value()}), 
            config.title
        );
    }
}

bool Window::isOpen()
{
    return mWindow.isOpen();
}

void Window::pollEvents()
{
    while (const std::optional event{mWindow.pollEvent()})
    {
        if (event->is<sf::Event::Closed>()) 
            mWindow.close();
    }
    
}

void Window::draw()
{
    mWindow.clear();
    mCHIP8.run();
    drawDisplay();
    mWindow.display();
}

void Window::drawDisplay()
{
    static sf::RectangleShape rect{{PIXEL_SIZE, PIXEL_SIZE}};
    const auto& chip8Display{mCHIP8.getDisplay()};
    for (u32 y{}; y < CHIP8::ROWS; ++y)
    {
        for (u32 x{}; x < CHIP8::COLS; ++x)
        {
            u32 index{y * CHIP8::COLS + x};
            rect.setFillColor(chip8Display[index] ? sf::Color::White : sf::Color::Black);
            rect.setPosition(
					{
						static_cast<f32>(x * PIXEL_SIZE), 
						static_cast<f32>(y * PIXEL_SIZE)
					});
            mWindow.draw(rect);
        }
    }
}

