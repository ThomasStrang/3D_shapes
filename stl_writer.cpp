#include "stl_writer.h"
#include <iomanip>
#include <string>

void writePoints(Tessellation* tess, std::ostream& outputStream) {
    //...TODO
}

void writeTriangles(Tessellation* tess, std::ostream& outputStream) {
    //...TODO
}

void StlWriter::writeTessellation(Tessellation* tess, std::ostream& outputStream) {
    outputStream << "#STL file" << std::endl;
    outputStream << std::endl;
    //...TODO
}
