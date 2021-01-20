#include "stl_writer.h"
#include <iomanip>
#include <string>

void writePoints(Tessellation* tess, std::ostream& outputStream) {
    //...TODO
}

void writeTriangles(Tessellation* tess, std::ostream& outputStream) {
    //...TODO
}

const Point subtract(const Point& a, const Point& b) {
    return Point(a.x-b.x, a.y-b.y, a.x-b.z);
}

const Point crossProduct(const Point& a, const Point& b) {
    return Point(a.y*b.x - a.z*b.y, a.x*b.x - a.z*b.x, a.x*b.y - a.y*b.x);
}

void StlWriter::writeTessellation(Tessellation* tess, std::ostream& outputStream) {
    outputStream << "solid " << std::endl;
    const std::vector<Triangle>& triangles = tess->allTriangles();
    const std::vector<Point>& points = tess->allPoints();
    const auto& indent = "    ";
    for (const Triangle& triangle : triangles) {
        const Point& a = points[triangle[0]];
        const Point& b = points[triangle[1]];
        const Point& c = points[triangle[2]];
        Point normal = crossProduct(subtract(b,a), subtract(c,a));
        outputStream << "facet " << "normal " << normal.x << " " << normal.y << " " << normal.z << std::endl;
        outputStream << indent << "outer loop" << std::endl;
        outputStream << indent << indent << "vertex " << a.x << " " << a.y << " " << a.z << std::endl;
        outputStream << indent << indent << "vertex " << b.x << " " << b.y << " " << b.z << std::endl;
        outputStream << indent << indent << "vertex " << c.x << " " << c.y << " " << c.z << std::endl;
        outputStream << indent << "endloop" << std::endl;
        outputStream << "endfacet"<< std::endl;
    }
    outputStream << "endsolid " << std::endl;
}
