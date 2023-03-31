#include <fstream>
#include "tessellation.h"
#include "stl_reader.h"
#include <iostream>
#include <unordered_map>

Point readPoint(std::istream& inputStream) {
	std::string str;
	inputStream >> str;
	std::cout << "We read this! " << str;
	if (str == "vertex") {
		double x, y, z;
		inputStream >> x;
		inputStream >> y;
		inputStream >> z;
		return Point(x, y, z);
	}
	else {
		std::cout << "ERROR reading point from input stream. Expected 'vertex' got '" << str << "'";
		throw std::runtime_error("ERROR reading point from input stream.");
	}
}

Triangle readTriangle(std::vector<Point>* points, std::unordered_map<Point, PointIndex>* point_indexes) {
	return Triangle();//TODO
}

std::unique_ptr<Tessellation> StlReader::readTessellation(std::istream& inputStream) {
	auto points = std::make_unique<std::vector<Point>>();
	auto triangles = std::make_unique<std::vector<Triangle>>();
	//std::unordered_map<Point, PointIndex> pointIndexes;
	//TODO do this
	return std::make_unique<Tessellation>(std::move(points), std::move(triangles));
}