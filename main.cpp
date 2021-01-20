#include "cxxopts.hpp"
#include "tessellation.h"
#include "ball_tessellation_generator.h"
#include "cube_tessellation_generator.h"
#include "stl_writer.h"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    //cxxopts::Options options(
    //    "BallTessellationGenerator",
    //    "Will generate a simple ball STL"
    //);
    //options.add_options()
    //    ("r,radius", "radius of the ball", cxxopts::value<double>())
    //    ("o,output", "output file name", cxxopts::value<std::string>());

    //auto args = options.parse(argc, argv);
    double radius = 100.00;
    std::string outputFileName = "output_file.STL";

    /*
    BallTessellationGenerator generator;
    auto tess = generator.makeBall(radius);*/
    //TODO THOMAS
    CubeTessellationGenerator generator;
    auto tess = generator.makeCube(radius);

    std::ofstream outputfilestream(outputFileName);
    StlWriter::writeTessellation(tess.get(), outputfilestream);
    outputfilestream.close();

    return 0;
}