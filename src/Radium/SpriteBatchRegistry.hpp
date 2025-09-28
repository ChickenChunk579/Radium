#pragma once
#include <Rune/SpriteBatch.hpp>
#include <iostream>

namespace Radium::SpriteBatchRegistry {
    Rune::SpriteBatch* Get(std::string name);
    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin, Rune::SamplingMode mode);
    std::vector<Rune::SpriteBatch*> GetAll();
}