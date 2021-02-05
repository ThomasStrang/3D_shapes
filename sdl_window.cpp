#include <SDL.h>
#include <iostream>
#include "sdl_window.h"


Window::Window() : initialised(false) {}

int Window::init() {
	if (initialised) return -1;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

	window = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>(SDL_CreateWindow("a hello world window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0), SDL_DestroyWindow);
	if (!window.get()) return -1;

	surface = std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface*)>>(SDL_GetWindowSurface(window.get()), SDL_FreeSurface);
	if (!surface.get()) return -1;

	pixels = (Uint32*)surface->pixels; //Might want to consider whether the surface ever changes it's pointer to pixels, for example if the window is resized?

	initialised = true;
	return 0;

}

bool Window::is_initialised() {
	return initialised;
}

int Window::get_w() {
	if (!is_initialised()) return -1;
	return surface->w;
}

int Window::get_h() {
	if (!initialised) return -1;
	return surface->h;
}

int Window::write_pixel(int x, int y, Uint8 r, Uint8 g, Uint8 b) {
	if (!initialised) return -1;
	Uint32 colour = 255 << 24 + r << 16 + g << 8 + b;
	pixels[x + y * surface->w] = colour;
	return 0;
}

int Window::write_pixel(int i, Uint8 r, Uint8 g, Uint8 b) {
	if (!initialised) return -1;
	Uint32 colour = 255 << 24 + r << 16 + g << 8 + b;
	pixels[i] = colour;
	return 0;
}

int Window::write_pixel(int i, Uint32 c) {
	if (!initialised) return -1;
	pixels[i] = c;
	return 0;
}

int Window::push_buffer() {
	if (!initialised) return -1;
	return SDL_UpdateWindowSurface(window.get());
}

Window::~Window() {
	surface.reset();
	window.reset();
	SDL_Quit();
}