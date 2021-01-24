#include <SDL.h>
#include <iostream>
#include "sdl_window.h"
namespace sdl_window {
    int sdl_window_program() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "Failed to initialize the SDL2 library\n";
            return -1;
        }

        SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 680, 480, 0);

        if (!window) {
            std::cout << "Failed to create window\n";
            return -1;
        }

        SDL_Surface* window_surface = SDL_GetWindowSurface(window);

        if (!window_surface) {
            std::cout << "Failed to get the surface from the window\n";
            return -1;
        }
        Uint32* pixels = (Uint32*)window_surface->pixels;
        int w = window_surface->w;
        int h = window_surface->h;
        auto* pixel_format = window_surface->format;
        
        bool stay_running = true;
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                pixels[x + y * w] = SDL_MapRGB(pixel_format, x * ((float)255 / w), 0, y * ((float)255 / h));
            }
        }
        bool ran = false;
        while (stay_running) {
            SDL_Delay(100);
            SDL_Event e;
            while (SDL_PollEvent(&e) > 0) {
                switch (e.type){
                case SDL_QUIT:
                    stay_running = false;
                    break;
                }
            }
            if (!ran)
            SDL_UpdateWindowSurface(window);
            ran = true;
        }
        return 0;
    }
}