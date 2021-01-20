#include "cube_tessellation_generator.h"
#include <iostream>



std::unique_ptr<Tessellation> CubeTessellationGenerator::makeCube(double length) {
	Point* points = (Point*)malloc(8 * sizeof(Point));
	Triangle* triangles = (Triangle*)malloc(12 * sizeof(Triangle));
	U32 trianglesPos = 0;
	for(int i = 0; i < 8; i++) {
		double x = i % 2;
		double y = (i >> 1) % 2;
		double z = (i >> 2) % 2;
		std::cout << x << "," << y << "," << z << std::endl;
		points[i] = Point(x*length, y*length, z*length);
	}

	triangles[0] = Triangle(0,2,1);
	triangles[1] = Triangle(3,1,2);
	triangles[4] = Triangle(0,4,2);
	triangles[5] = Triangle(6,2,4);
	triangles[2] = Triangle(5,7,4);
	triangles[3] = Triangle(6,4,7);
	triangles[6] = Triangle(1,3,5);
	triangles[7] = Triangle(7,5,3);
	triangles[8] = Triangle(2,6,3);
	triangles[9] = Triangle(7,3,6);
	triangles[10] = Triangle(4,0,5);
	triangles[11] = Triangle(1,5,0);

	return std::make_unique<Tessellation>(points, 8, triangles, 12);
}