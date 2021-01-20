#pragma once
#include <memory>
#include "tessellation.h"

class CubeTessellationGenerator {
public:
    std::unique_ptr<Tessellation> makeCube(double length);
};
