#include <Radium/AssetLoader.hpp>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <algorithm> // for std::replace

namespace Radium
{
    std::string assetBase = "";

    std::string ReadFileToString(std::string filename, bool external)
    {
        spdlog::info("Asset base: {}", assetBase);
        std::string expandedFileName = assetBase + filename;

        // Use backslashes on Windows (MSVC)
#ifdef _MSC_VER
        std::replace(expandedFileName.begin(), expandedFileName.end(), '/', '\\');
#endif

        spdlog::trace("Reading file: {}", expandedFileName);

        SDL_RWops *file = SDL_RWFromFile(expandedFileName.c_str(), "rb");

        

        // Try as an asset if not found on filesystem (Android case or fallback)
        if (!file)
        {
#ifdef _MSC_VER
            std::string altPath = "assets\\" + filename;
            std::replace(altPath.begin(), altPath.end(), '/', '\\');
#else
            std::string altPath = "assets/" + filename;
#endif
            spdlog::trace("Attempting odd read");
            file = SDL_RWFromFile(altPath.c_str(), "rb");
        } else {
            spdlog::trace("Opened");
        }

        if (!file)
        {
            std::cerr << "Failed to open file: " << expandedFileName << "\n";
            return {};
        }

        Sint64 res_size = SDL_RWsize(file);
        spdlog::trace("Got size: {}", res_size);
        std::string content;
        content.resize(res_size);

        Sint64 nb_read_total = 0, nb_read = 1;
        char *buf = &content[0];

        while (nb_read_total < res_size && nb_read != 0)
        {
            spdlog::trace("Reading...");
            nb_read = SDL_RWread(file, buf + nb_read_total, 1, res_size - nb_read_total);
            nb_read_total += nb_read;
        }

        SDL_RWclose(file);
        spdlog::trace("Closed");

        if (nb_read_total != res_size)
        {
            std::cerr << "Read error\n";
            return {};
        }

        return content;
    }
}