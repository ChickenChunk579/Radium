#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/AssetLoader.hpp>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <Flux/Flux.hpp>

namespace Radium::SpriteBatchRegistry {
    std::unordered_map<std::string, Rune::SpriteBatch*> map;

    Rune::SpriteBatch* Get(std::string name) {
        return map[name];
    }

    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin, Rune::SamplingMode mode) {
        std::string resolvedPath = Radium::assetBase + texturePath;
        SDL_Surface* surface = IMG_Load(resolvedPath.c_str());

        Flux::Info("Loading texture at {}", resolvedPath);

        if (!surface) {
            Flux::Error("Failed to load image: {}", SDL_GetError());
            return;
        }
        
        // Convert to 32-bit RGBA
        SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);

        int w = converted->w;
        int h = converted->h;
        uint8_t* pixels = new uint8_t[w * h * 4]; // tightly packed
        
        for (int y = 0; y < h; ++y) {
            uint8_t* srcRow = (uint8_t*)((uint8_t*)converted->pixels + y * converted->pitch);
            memcpy(pixels + y * w * 4, srcRow, w * 4);
        }
        
        // Now pixels is a tightly packed RGBA buffer
        Rune::Texture* texture = new Rune::Texture(w, h, w*4, pixels, mode);

        delete[] pixels;

        // Free surfaces
        SDL_FreeSurface(converted);
        SDL_FreeSurface(surface);


        Rune::SpriteBatch* batch = new Rune::SpriteBatch(texture, origin);
        map[name] = batch;
    }


    void Add(std::string name, Rune::Texture* texture, Rune::SpriteOrigin origin, Rune::SamplingMode mode) {
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

    void Clear() {
        map.clear();
    }
}