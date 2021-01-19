#include "cxxopts.hpp"
#include "tessellation.h"
#include "ball_tessellation_generator.h"
#include "stl_writer.h"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "Hello World!";

    cxxopts::Options options(
        "BallTessellationGenerator",
        "Will generate a simple ball STL"
    );
    options.add_options()
        ("r,radius", "radius of the ball", cxxopts::value<double>())
        ("o,output", "output file name", cxxopts::value<std::string>());

    auto args = options.parse(argc, argv);
    auto radius = args["radius"].as<double>();
    auto outputFileName = args["output"].as<std::string>();

    BallTessellationGenerator generator;
    auto tess = generator.makeBall(radius);

    std::cout << "got here!";

    ////TODO Thomas
    ////CubeTessellationGenerator generator;
    ////auto tess = generator.makeCube(radius);

    //std::ofstream outputFileStream(outputFileName);
    //StlWriter::writeTessellation(tess.get(), outputFileStream);
    //outputFileStream.close();

    return 0;
}