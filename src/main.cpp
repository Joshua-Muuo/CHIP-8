#include <print>
#include "window.hpp"

int main()
{
	std::println("Hello World!");

	Window window({.title = "CHIP8"});
	while (window.isOpen())
	{
		window.pollEvents();
		window.draw();
	}

	return 0;
}    
