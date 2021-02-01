#include "cxxopts.hpp"
#include "tessellation.h"
#include "ball_tessellation_generator.h"
#include "cube_tessellation_generator.h"
#include "stl_writer.h"
#include "stl_reader.h"
#include "sdl_window.h"
#include "frame_controller.h"
#include <fstream>
#include <iostream>
#include "renderer.cpp"

int tessellation_program(int argc, char** argv) {
    CubeTessellationGenerator generator;
    auto tess = generator.makeFractalCube(100, 1);
    std::ofstream outputfilestream("output_file.STL");
    StlWriter::writeTessellation(tess.get(), outputfilestream);
    outputfilestream.close();
    return 0;
}

  

int main(int argc, char** argv) {
    tessellation_program(argc, argv);
    //return 0;
    Window window;
    if (window.init() == 0) {

        //std::ifstream inputfilestream("cb750.STL");
        //auto tess = StlReader.readTessellation(inputfilestream);
        CubeTessellationGenerator generator;
        auto tess = generator.makeFractalCube(100, 2);
        Renderer r = Renderer(window, tess.get());
        r.init();
        auto frameController = FrameController(60);
        bool quit = false;
        SDL_Event e;

        while (!quit) {
            if (SDL_PollEvent(&e) > 0) {
                switch (e.type) {
                case SDL_QUIT :
                    quit = true;
                    break;
                case SDL_MOUSEWHEEL :
                    r.move_camera(Matrix4x1(0, 25*(double)e.wheel.y, 0, 0));
                    break;
                case SDL_KEYDOWN :
                    switch (e.key.keysym.sym) {
                    case SDLK_w:
                        r.move_camera(Matrix4x1(0, 0, 25.0, 0));
                        break;
                    case SDLK_a:
                        r.move_camera(Matrix4x1(-25, 0, 0, 0));
                        break;
                    case SDLK_s:
                        r.move_camera(Matrix4x1(0, 0, -25.0, 0));
                        break;
                    case SDLK_d:
                        r.move_camera(Matrix4x1(25, 0, 0, 0));
                        break;
                    case SDLK_LEFT:
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_UP:
                        break;
                    case SDLK_DOWN:
                        break;
                    }
                    break;
                }
            } else {
                auto state = SDL_GetKeyboardState(NULL);
                if (state[SDL_SCANCODE_UP])
                    r.move_camera(Matrix4x1(0, 5, 0, 0));
                if (state[SDL_SCANCODE_DOWN])
                    r.move_camera(Matrix4x1(0, -5, 0, 0));
                if (state[SDL_SCANCODE_LEFT])
                    r.move_camera(Matrix4x1(-5, 0, 0, 0));
                if (state[SDL_SCANCODE_RIGHT])
                    r.move_camera(Matrix4x1(5, 0, 0, 0));
                r.render();
                window.push_buffer();
                frameController.wait_next_frame();
            }
        }
    }
    return 0;
}