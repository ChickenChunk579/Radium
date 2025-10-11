#pragma once
#include <Rune/SpriteBatch.hpp>
#include <Rune/Texture.hpp>
#include <iostream>

namespace Radium::SpriteBatchRegistry {
    /// @brief Get a sprite batch from the registry by its tag
    Rune::SpriteBatch* Get(std::string name);

    /// @brief Add a texture file to the registry as a batch
    void Add(std::string name, std::string texturePath, Rune::SpriteOrigin origin, Rune::SamplingMode mode);
    
    /// @brief Add a texture from a Rune texture to the registry as a batch
    void Add(std::string name, Rune::Texture* texture, Rune::SpriteOrigin origin, Rune::SamplingMode mode);
    
    /// @brief Get all batches in the registty
    std::vector<Rune::SpriteBatch*> GetAll();

    /// @brief Clear the registry, freeing all batches
    void Clear();
}