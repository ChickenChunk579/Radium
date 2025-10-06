#pragma once
#include <iostream>

namespace Radium {
    extern std::string assetBase;

    std::string ReadFileToString(std::string filename, bool external = false);
}