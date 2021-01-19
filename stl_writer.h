#pragma once

#include <fstream>
#include "tessellation.h"

class StlWriter {
public:
    static void writeTessellation(Tessellation* tess, std::ostream& outputStream);
};