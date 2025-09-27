#pragma once
#include <Rune/SpriteBatch.hpp>
#include <iostream>

namespace Radium::SpriteBatchRegistry {
    Rune::SpriteBatch* Get(std::string name);
    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin);
    std::vector<Rune::SpriteBatch*> GetAll();
}