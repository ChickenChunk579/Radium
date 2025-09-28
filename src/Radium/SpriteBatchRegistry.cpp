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
            spdlog::info("ANDROID123 Error: {}", IMG_GetError());
            
        }
        int w = surface->w;
        int h = surface->h;
        void* pixels = surface->pixels;

        Rune::Texture* texture = new Rune::Texture(w, h, pixels, mode);

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