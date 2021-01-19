#include "ball_tessellation_generator.h"
#define _USE_MATH_DEFINES
#include <math.h>


std::unique_ptr<Tessellation> BallTessellationGenerator::makeBall(double r) {
    U32 k = 0;
    const U32 numPoints = 2 * N * (N + 1);
    Point* points = (Point*)malloc(numPoints * sizeof(Point));
    PointIndex ps[N + 1][2 * N];
    const U32 numTriangles = 4 * N * (N - 1);
    Triangle* triangles = (Triangle*)malloc(numTriangles * sizeof(Triangle));

    for (int i = 0; i <= N; i++) {
        double theta = i * M_PI / N;
        for (int j = 0; j < (2 * N); j++) {
            double phi = j * M_PI / N;
            double x = r * sin(theta) * cos(phi);
            double y = r * sin(theta) * sin(phi);
            double z = r * cos(theta);
            points[k] = Point(x, y, z);
            ps[i][j] = k;
            k++;
        }
    }

    k = 0;
    for (int i = 1; i <= N; i++) {
        for (int j = 0; j < 2 * N; j++) {
            PointIndex p1 = ps[i][j % (2 * N)];
            PointIndex p2 = ps[i][(j + 1) % (2 * N)];
            PointIndex p3 = ps[i - 1][j % (2 * N)];
            PointIndex p4 = ps[i - 1][(j + 1) % (2 * N)];
            if (i < N) {
                triangles[k] = Triangle(p1, p2, p3);
                k++;
            }
            if (i > 1) {
                triangles[k] = Triangle(p2, p4, p3);
                k++;
            }
        }
    }

    return std::make_unique<Tessellation>(points, numPoints, triangles, numTriangles);
}