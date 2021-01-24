#pragma once
#include <memory>
#include "tessellation.h"
#include <unordered_map>

class CubeTessellationGenerator {
public:
    std::unique_ptr<Tessellation> makeCube(double length);
    std::unique_ptr<Tessellation> CubeTessellationGenerator::makeFractalCube(double length, int d);
    void CubeTessellationGenerator::makeFractalCubeRecurse(int d, Point location, double length, std::vector<Point>* points, std::vector<Triangle>* triangles, std::unordered_map<Point, PointIndex>& pointIndexes);
};
