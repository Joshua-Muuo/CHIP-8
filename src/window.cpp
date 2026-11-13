#include "window.hpp"

Window::Window(const Config& config):
mWindow{sf::VideoMode({config.width, config.height}), config.title}
{
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
    mWindow.clear(sf::Color::Red);
    return;
}
