#pragma once
#include <memory>
#include "tessellation.h"

class BallTessellationGenerator {
public:
    std::unique_ptr<Tessellation> makeBall(double radius);
private:
    //The quality of the tesselation. Specifically the number of straight
    //line segments in an arc spanning pi radians around the ball.
    static const U32 N = 50;
};
