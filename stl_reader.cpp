#include <fstream>
#include "tessellation.h"
#include "stl_reader.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
using namespace std;


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

Point readPoint(istream& inputStream) {
	string str;
	inputStream >> str;
	cout << "We read this! " << str;
	if (str == "vertex") {
		double x, y, z;
		inputStream >> x;
		inputStream >> y;
		inputStream >> z;
		return Point(x, y, z);
	}
	else {
		cout << "ERROR reading point from input stream. Expected 'vertex' got '" << str << "'";
		throw runtime_error("ERROR reading point from input stream.");
	}
}

Triangle readTriangle(vector<Point>* points, unordered_map<Point, PointIndex>* point_indexes) {
	return Triangle();//TODO
}

bool whitespace(char c) {
	return c == ' ' || c == '\n' || c == '\t';
}

bool getlinelstripped(istream& inputStream, string& line) {
	if (getline(inputStream, line)) {
		int start_index = 0;
		while (start_index < line.length() && whitespace(line[start_index])) {
			start_index++;
		}
		if (start_index == line.length()) {
			line = "";
		}
		else {
			line = line.substr(start_index, line.length() - start_index);
		}
		return true;
	}
	else {
		return false;
	}
}

 bool getLineWords(istream& inputStream, vector<string>& words) {
	string line;
	if (!getlinelstripped(inputStream, line)) return false;
	istringstream line_stream(line);
	string word;
	words.clear();
	while (line_stream >> word) {
		if (word != "") {
			words.push_back(word);
		}
	}
	return true;
}

 PointIndex getIndexAndAddIfNew(Point& p, std::vector<Point>* points, std::unordered_map<Point, PointIndex>& pointIndexes) {
	 auto it = pointIndexes.find(p);
	 if (it == pointIndexes.end()) {
		 PointIndex pindex = points->size();
		 points->push_back(p);
		 pointIndexes[p] = pindex;
	 }
	 return pointIndexes[p];
 }

 bool parseTriangle(istream& inputStream, std::vector<Point>* points, std::vector<Triangle>* triangles, std::unordered_map<Point, PointIndex>& pointIndexes) {
	 vector<string> words;
	 if (!getLineWords(inputStream, words) || words[0] != "outer")
		 return false;
	 if (!getLineWords(inputStream, words) || words[0] != "vertex" || words.size() != 4)
		 return false;
	 Point a = Point(stod(words[1]), stod(words[2]), stod(words[3]));
	 if (!getLineWords(inputStream, words) || words[0] != "vertex" || words.size() != 4)
		 return false;
	 Point b = Point(stod(words[1]), stod(words[2]), stod(words[3]));
	 if (!getLineWords(inputStream, words) || words[0] != "vertex" || words.size() != 4)
		 return false;
	 Point c = Point(stod(words[1]), stod(words[2]), stod(words[3]));
	 PointIndex a_index = getIndexAndAddIfNew(a, points, pointIndexes);
	 PointIndex b_index = getIndexAndAddIfNew(b, points, pointIndexes);
	 PointIndex c_index = getIndexAndAddIfNew(c, points, pointIndexes);
	 triangles->push_back(Triangle(a_index, b_index, c_index));
	 if (!getLineWords(inputStream, words) || words[0] != "endloop")
		 return false;
	 if (!getLineWords(inputStream, words) || words[0] != "endfacet")
		 return false;
	 return true;
 }

bool parseSolid(istream& inputStream, std::vector<Point>* points, std::vector<Triangle>* triangles, std::unordered_map<Point, PointIndex>& pointIndexes) {
	vector<string> words;
	if (!getLineWords(inputStream, words) || words[0] != "solid")
		return false;
	while (getLineWords(inputStream, words)) {
		if (words[0] == "facet") {
			if (!parseTriangle(inputStream, points, triangles, pointIndexes)) return false;
		} else if (words[0]=="endsolid") {
			return true;
		}
	}
	return false;
}

unique_ptr<Tessellation> StlReader::readTessellation(istream& inputStream) {
	auto points = make_unique<vector<Point>>();
	auto triangles = make_unique<vector<Triangle>>();
	
	
	std::unordered_map<Point, PointIndex> pointIndexes;
	if (parseSolid(inputStream, points.get(), triangles.get(), pointIndexes)) {
		cout << "WE DID IT!!!\n";
	}
	else {
		cout << "We failed... Time to add some logging.\n";
		getchar();
	}
	return make_unique<Tessellation>(move(points), move(triangles));
}