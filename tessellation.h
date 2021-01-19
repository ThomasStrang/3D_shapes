#pragma once
#include <memory>
#include <vector>

typedef unsigned long int U32;
typedef U32 PointIndex;



struct Point {
    double x, y, z;
    Point(double x, double y, double z) : x(x), y(y), z(z) {};
    Point() : x(), y(), z() {};
};

struct Triangle {
    PointIndex ps[3];
    Triangle(PointIndex p0, PointIndex p1, PointIndex p2) {
        ps[0] = p0; ps[1] = p1; ps[2] = p2;
    }
    Triangle() : ps() {};
    PointIndex operator[](U32 i) const {
        return ps[i];
    }
};

struct Tessellation {
public:
    Tessellation(
        std::unique_ptr<std::vector<Point>> points,
        std::unique_ptr<std::vector<Triangle>> triangles
    ) :
        m_points(std::move(points)),
        m_triangles(std::move(triangles))
    {}
    Tessellation(
        Point* points, U32 numPoints,
        Triangle* triangles, U32 numTriangles
    ) :
        m_points(std::make_unique<std::vector<Point>>(points, points + numPoints)),
        m_triangles(std::make_unique<std::vector<Triangle>>(triangles, triangles + numTriangles))
    {}
    const std::vector<Point>& allPoints() const { return *m_points; }
    const std::vector<Triangle>& allTriangles() const { return *m_triangles; }
private:
    std::unique_ptr<std::vector<Point>>    m_points;
    std::unique_ptr<std::vector<Triangle>> m_triangles;
};
