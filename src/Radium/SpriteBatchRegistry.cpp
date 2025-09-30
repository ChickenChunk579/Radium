#include <Radium/SpriteBatchRegistry.hpp>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <spdlog/spdlog.h>

namespace Radium::SpriteBatchRegistry {
    std::unordered_map<std::string, Rune::SpriteBatch*> map;

    Rune::SpriteBatch* Get(std::string name) {
        return map[name];
    }

    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin, Rune::SamplingMode mode) {
        #ifndef __ANDROID__
        std::string fullPath = "assets/" + texturePath;
        const char* path = fullPath.c_str();

        spdlog::info("Resolved texture path: {}", path);
        SDL_Surface* surface = IMG_Load(path);
        #else
        SDL_Surface* surface = IMG_Load(texturePath.c_str());
        #endif


        if (!surface) {
            spdlog::error("Failed to convert surface to RGBA8888: {}", SDL_GetError());
            return; // Or handle error
        }

        spdlog::info("Loaded surface width: {}, height: {}, format: {}", surface->w, surface->h, SDL_GetPixelFormatName(surface->format->format));
        spdlog::info("Bytes per pixel: {}", surface->format->BytesPerPixel);
        spdlog::info("Surface pitch: {}", surface->pitch);


        int w = surface->w;
        int h = surface->h;
        uint8_t* pixels = (uint8_t*)surface->pixels;

 
        // Now pass pixels to your texture (RGBA8 data)
        Rune::Texture* texture = new Rune::Texture(w, h, surface->pitch, surface->pixels, mode);

        // Don't forget to free the surface after texture creation (if texture copies data internally)
        SDL_FreeSurface(surface);

        Rune::SpriteBatch* batch = new Rune::SpriteBatch(texture, origin);
        map[name] = batch;
    }

    std::vector<Rune::SpriteBatch*> GetAll() {
        std::vector<Rune::SpriteBatch*> batches;

        for (auto pair : map) {
            batches.push_back(pair.second);
        }

        return batches;
    }
}