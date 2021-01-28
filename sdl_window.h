#pragma once
#include <functional>
#include <SDL.h>

/// <summary>
/// Class to provide an interface with SDL window buffer, and to have some helper functions
/// </summary>
class Window {

private:
	std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window;
	std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface*)>> surface;
	Uint32* pixels;
	bool initialised;

public:
	Window();
	int init();
	bool is_initialised();
	int get_w();
	int get_h();
	int write_pixel(int x, int y, Uint8 r, Uint8 g, Uint8 b);
	int push_buffer();
	~Window();
};