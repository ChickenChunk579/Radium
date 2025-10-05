#pragma once
#include <Rune/SpriteBatch.hpp>
#include <Rune/Texture.hpp>
#include <iostream>

namespace Radium::SpriteBatchRegistry {
    Rune::SpriteBatch* Get(std::string name);
    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin, Rune::SamplingMode mode);
    void Add(std::string name, Rune::Texture* texture, Rune::SpriteOrigin origin, Rune::SamplingMode mode);
    std::vector<Rune::SpriteBatch*> GetAll();

    void Clear();
}