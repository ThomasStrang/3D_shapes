
#include "cube_tessellation_generator.h"
#include <iostream>
#include <unordered_map>

namespace std {
	template<> struct hash<Point> {
		std::size_t operator()(const Point& point) const noexcept {
			std::hash<double> doublehasher;
			std::size_t hx = doublehasher(point.x);
			std::size_t hy = doublehasher(point.y);
			std::size_t hz = doublehasher(point.z);
			return hx ^ ((hy ^ (hz << 1)) << 1);
		}
	};
}

std::unique_ptr<Tessellation> CubeTessellationGenerator::makeCube(double length) {
	return makeFractalCube(length, 0);
}

std::unique_ptr<Tessellation> CubeTessellationGenerator::makeFractalCube(double length, int d) {
	auto points = std::make_unique<std::vector<Point>>();
	auto triangles = std::make_unique<std::vector<Triangle>>();
	std::unordered_map<Point, PointIndex> pointIndexes;
	makeFractalCubeRecurse(d, Point(0, 0, 0), length, points.get(), triangles.get(), pointIndexes);
	return std::make_unique<Tessellation>(std::move(points), std::move(triangles));
}

void CubeTessellationGenerator::makeFractalCubeRecurse(int d, Point location, double length, std::vector<Point>* points, std::vector<Triangle>* triangles, std::unordered_map<Point, PointIndex>& pointIndexes) {
	if (d == 0) {
		//we are done recursing, this is the base level.
		PointIndex bds[8];
		for (int z = 0; z < 2; z++) {
			for (int y = 0; y < 2; y++) {
				for (int x = 0; x < 2; x++) {
					Point p = Point(location.x + x * length, location.y + y * length, location.z + z * length);
					auto it = pointIndexes.find(p);
					if( it == pointIndexes.end()) {
						PointIndex pindex = points->size();
						points->push_back(p);
						pointIndexes[p]= pindex;
						bds[x + (y * 2) + (z * 4)] = pindex;
					} else {
						bds[x + (y * 2) + (z * 4)] = it->second;
					}
				}
			}
		}
		triangles->push_back(Triangle(bds[0], bds[2], bds[1]));
		triangles->push_back(Triangle(bds[3], bds[1], bds[2]));
		triangles->push_back(Triangle(bds[0], bds[4], bds[2]));
		triangles->push_back(Triangle(bds[6], bds[2], bds[4]));
		triangles->push_back(Triangle(bds[5], bds[7], bds[4]));
		triangles->push_back(Triangle(bds[6], bds[4], bds[7]));
		triangles->push_back(Triangle(bds[1], bds[3], bds[5]));
		triangles->push_back(Triangle(bds[7], bds[5], bds[3]));
		triangles->push_back(Triangle(bds[2], bds[6], bds[3]));
		triangles->push_back(Triangle(bds[7], bds[3], bds[6]));
		triangles->push_back(Triangle(bds[4], bds[0], bds[5]));
		triangles->push_back(Triangle(bds[1], bds[5], bds[0]));
	} else {
		//recurse on sub cubes.
		double sub_length = length / 3.0;
		bool every_second = true; 
		for (double z = 0; z < 3; z++) {
			for (double y = 0; y < 3; y++) {
				for (double x = 0; x < 3; x++) {
					if (every_second) {
						Point sub_location = Point(location.x+(x*sub_length), location.y + (y*sub_length), location.z + (z*sub_length));
						makeFractalCubeRecurse(d - 1, sub_location, sub_length, points, triangles, pointIndexes);
					}
					every_second = !every_second;
				}
			}
		}
	}
}

