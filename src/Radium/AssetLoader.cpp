#include <Radium/AssetLoader.hpp>
#include <SDL2/SDL.h>
#include <Flux/Flux.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace Radium
{
    std::string assetBase = "";

    std::string ReadFileToString(std::string filename, bool external)
    {
        Flux::Info("Asset base: {}", assetBase);
        std::string expandedFileName = assetBase + filename;

    #ifdef _MSC_VER
        std::replace(expandedFileName.begin(), expandedFileName.end(), '/', '\\');
    #endif

        std::ifstream file(expandedFileName, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            Flux::Error("Failed to open file: {}", expandedFileName);
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf(); // Read entire file contents
        file.close();

        std::string contents = buffer.str();
        Flux::Info("Read file '{}' ({} bytes)", expandedFileName, contents.size());

        return contents;
    }


}