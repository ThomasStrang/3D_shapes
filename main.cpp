
#include <fstream>
#include <iostream>
#include "renderer.cpp"
#include "cxxopts.hpp"
#include "tessellation.h"
#include "ball_tessellation_generator.h"
#include "cube_tessellation_generator.h"
#include "stl_writer.h"
#include "stl_reader.h"
#include "sdl_window.h"
#include "frame_controller.h"
#include "camera.h"

int tessellation_program(int argc, char** argv) {
    CubeTessellationGenerator generator;
    auto tess = generator.makeFractalCube(100, 1);
    std::ofstream outputfilestream("output_file.STL");
    StlWriter::writeTessellation(tess.get(), outputfilestream);
    outputfilestream.close();
    return 0;
}

void handle_keyboard_input(Camera& c) {
    auto state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP])
        c.move_camera_relative_to_rotation(Matrix4x1(0, 0, 5, 0));
    if (state[SDL_SCANCODE_DOWN])
        c.move_camera_relative_to_rotation(Matrix4x1(0, 0, -5, 0));
    if (state[SDL_SCANCODE_LEFT])
        c.move_camera_relative_to_rotation(Matrix4x1(-5, 0, 0, 0));
    if (state[SDL_SCANCODE_RIGHT])
        c.move_camera_relative_to_rotation(Matrix4x1(5, 0, 0, 0));
    if (state[SDL_SCANCODE_W])
        c.rotate_camera_pitch(0.1);
    if (state[SDL_SCANCODE_S])
        c.rotate_camera_pitch(-0.1);
    if (state[SDL_SCANCODE_A])
        c.rotate_camera_roll(0.1);
    if (state[SDL_SCANCODE_D])
        c.rotate_camera_roll(-0.1);
    if (state[SDL_SCANCODE_Q])
        c.rotate_camera_yaw(-0.1);
    if (state[SDL_SCANCODE_E])
        c.rotate_camera_yaw(0.1);
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
        Camera c = Camera();
        Renderer r = Renderer(window, &c, tess.get());
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
                }
            } else {
                handle_keyboard_input(c);
                r.render();
                window.push_buffer();
                frameController.wait_next_frame();
            }
        }
    }
    return 0;
}