#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/AssetLoader.hpp>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <Iris/Iris.hpp>
#include <Flux/Flux.hpp>

namespace Radium::SpriteBatchRegistry {
    std::unordered_map<std::string, Rune::SpriteBatch*> map;

    Rune::SpriteBatch* Get(std::string name) {
        return map[name];
    }

    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin, Rune::SamplingMode mode) {
        std::string resolvedPath = Radium::assetBase + texturePath;
        Iris::Image image = Iris::Image::Load(resolvedPath);

        Flux::Info("Loading texture at {}", resolvedPath);

        Rune::Texture* texture = new Rune::Texture(image.width, image.height, 4 * image.width, image.data.data(), Rune::SamplingMode::Nearest);
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