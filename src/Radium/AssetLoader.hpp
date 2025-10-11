#pragma once
#include <iostream>

namespace Radium {
    /// @brief Base for all assets to be loaded from.
    extern std::string assetBase;

    /**
     * @brief Read a file from assetfs to a string
     * 
     * @param filename A path to a file
     * @param external Whether the path should have assetBase or not.
     */
    std::string ReadFileToString(std::string filename, bool external = false);
}