#include "cxxopts.hpp"
#include "tessellation.h"
#include "ball_tessellation_generator.h"
#include "cube_tessellation_generator.h"
#include "stl_writer.h"
#include "sdl_window.h"
#include "frame_controller.h"
#include <fstream>
#include <iostream>

int tessellation_program(int argc, char** argv) {
    CubeTessellationGenerator generator;
    auto tess = generator.makeFractalCube(128, 2);
    std::ofstream outputfilestream("output_file.STL");
    StlWriter::writeTessellation(tess.get(), outputfilestream);
    outputfilestream.close();
    return 0;
}

int main(int argc, char** argv) {
    Window window;
    if (window.init() == 0) {

        BallTessellationGenerator generator;
        auto sphere = generator.makeBall(100);
        int w = window.get_w();
        int h = window.get_h();
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                window.write_pixel(x, y, x * ((float)255 / w), 0, y * ((float)255 / h));
            }
        }

        auto frameController = FrameController(60);
        bool quit = false;
        SDL_Event e;

        while (!quit) {
            if (SDL_PollEvent(&e) > 0) {
                switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                }
            }
            else {
                window.push_buffer();
                frameController.wait_next_frame();
            }
        }
    }
    return 0;
}