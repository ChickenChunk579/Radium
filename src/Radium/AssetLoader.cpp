#include <Radium/AssetLoader.hpp>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>
namespace Radium
{
    std::string assetBase = "";

    std::string ReadFileToString(std::string filename, bool external)
    {
        spdlog::info("Asset base: {}", assetBase);
        std::string expandedFileName = assetBase + filename;
        


        SDL_RWops *file = SDL_RWFromFile(expandedFileName.c_str(), "rb");

        // Try as an asset if not found on filesystem (Android case)
        if (!file)
        {
            file = SDL_RWFromFile((std::string("assets/") + filename).c_str(), "rb");
        }

        if (!file)
        {
            std::cerr << "Failed to open file: " << expandedFileName << "\n";
            return {};
        }

        Sint64 res_size = SDL_RWsize(file);
        std::string content;
        content.resize(res_size);

        Sint64 nb_read_total = 0, nb_read = 1;
        char *buf = &content[0];

        while (nb_read_total < res_size && nb_read != 0)
        {
            nb_read = SDL_RWread(file, buf + nb_read_total, 1, res_size - nb_read_total);
            nb_read_total += nb_read;
        }

        SDL_RWclose(file);

        if (nb_read_total != res_size)
        {
            std::cerr << "Read error\n";
            return {};
        }

        return content;
    }
}