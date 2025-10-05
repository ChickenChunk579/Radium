#include <Radium/SpriteBatchRegistry.hpp>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <spdlog/spdlog.h>

namespace Radium::SpriteBatchRegistry
{
    std::unordered_map<std::string, Rune::SpriteBatch *> map;
    // Keep track of textures we create from image files so we can destroy them on Clear()
    static std::unordered_map<std::string, Rune::Texture *> createdTextures;

    Rune::SpriteBatch *Get(std::string name)
    {
        return map[name];
    }

    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin, Rune::SamplingMode mode)
    {
#if !defined(__ANDROID__)
        std::string fullPath = "assets/" + texturePath;
        const char *path = fullPath.c_str();

        spdlog::info("Resolved texture path: {}", path);
        SDL_Surface *surface = IMG_Load(path);
#else
        SDL_Surface *surface = IMG_Load(texturePath.c_str());
#endif

        if (!surface)
        {
            spdlog::error("Failed to load image: {}", SDL_GetError());
            return;
        }

        // Convert to 32-bit RGBA
        SDL_Surface *converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);

        int w = converted->w;
        int h = converted->h;
        uint8_t *pixels = new uint8_t[w * h * 4]; // tightly packed

        for (int y = 0; y < h; ++y)
        {
            uint8_t *srcRow = (uint8_t *)((uint8_t *)converted->pixels + y * converted->pitch);
            memcpy(pixels + y * w * 4, srcRow, w * 4);
        }

        

        // Now pixels is a tightly packed RGBA buffer
        Rune::Texture *texture = new Rune::Texture(w, h, w * 4, pixels, mode);

        // The Texture uploads the pixel data into GPU memory; we no longer need the CPU buffer.
        delete[] pixels;

        // Free surfaces
        SDL_FreeSurface(converted);
        SDL_FreeSurface(surface);

        Rune::SpriteBatch *batch = new Rune::SpriteBatch(texture, origin);
        map[name] = batch;
        createdTextures[name] = texture;
    }

    void Add(std::string name, Rune::Texture *texture, Rune::SpriteOrigin origin, Rune::SamplingMode mode)
    {
        Rune::SpriteBatch *batch = new Rune::SpriteBatch(texture, origin);
        map[name] = batch;
    }

    std::vector<Rune::SpriteBatch *> GetAll()
    {
        std::vector<Rune::SpriteBatch *> batches;

        for (auto pair : map)
        {
            batches.push_back(pair.second);
        }

        return batches;
    }

    void Clear()
    {
        // Destroy and delete all sprite batches
        for (auto &p : map)
        {
            Rune::SpriteBatch *batch = p.second;
            if (batch)
            {
                batch->Destroy();
                delete batch;
            }
        }
        map.clear();

        // Destroy and delete any textures we created from image files
        for (auto &p : createdTextures)
        {
            Rune::Texture *tex = p.second;
            if (tex)
            {
                tex->Destroy();
                delete tex;
            }
        }
        createdTextures.clear();
    }
}