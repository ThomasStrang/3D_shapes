#pragma once

#include <fstream>
#include "tessellation.h"

class StlReader {
public:
    static std::unique_ptr<Tessellation> readTessellation(std::istream& inputStream);
};
