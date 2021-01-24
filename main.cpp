#include "cxxopts.hpp"
#include "tessellation.h"
#include "ball_tessellation_generator.h"
#include "cube_tessellation_generator.h"
#include "stl_writer.h"
#include "sdl_window.h"
#include <fstream>
#include <iostream>

int tessellation_program(int argc, char** argv) {
    //cxxopts::Options options(
    //    "BallTessellationGenerator",
    //    "Will generate a simple ball STL"
    //);
    //options.add_options()
    //    ("r,radius", "radius of the ball", cxxopts::value<double>())
    //    ("o,output", "output file name", cxxopts::value<std::string>());

    //auto args = options.parse(argc, argv);
    double radius = 128;
    int depth = 2;
    std::string outputFileName = "output_file.STL";

    /*
    BallTessellationGenerator generator;
    auto tess = generator.makeBall(radius);*/
    CubeTessellationGenerator generator;
    auto tess = generator.makeFractalCube(radius, depth);

    std::cout << "Generated Tessellation." << std::endl;
    std::ofstream outputfilestream(outputFileName);
    StlWriter::writeTessellation(tess.get(), outputfilestream);
    outputfilestream.close();
    std::cout << "Wrote Tessellation to file.";
    return 0;
}

int main(int argc, char** argv) {
    //return tessellation_program(argc,argv);
    sdl_window::sdl_window_program();
}