#pragma once

#include <Radium/Nodes/2D/Node2D.hpp>
#include <Rune/SpriteBatch.hpp>

namespace Radium::Nodes {
    /// @brief One chunk in the tilemap
    class TileChunk {
    private:
        Vector2i* data;

    public:
        static const int width = 16;
        static const int height = 16;

        Rune::Texture* texture;

        /// @brief The size of the tiles
        Vector2i tileSize;

        /// @brief The offset from the top left of the sprite where the sprites start
        Vector2i tileOffset;

        /// @brief Seperation in pixels between the files
        Vector2i tileSeperation;

        /// @brief Create a tilemap chunk with a SDL_Surface source texture
        TileChunk(void* source, Vector2i tileSize, Vector2i tileOffset, Vector2i tileSeperation);

        /**
         * @brief Set a tile at a position in the chunk
         * 
         * @param tilePosition The position of the source tile in the tilemap
         * @param destination Where to place the file on the tilemap
         */
        void SetTile(Vector2i tilePosition, Vector2i destination);

        /// @brief Generate the texture for the tile chunk
        void GenerateTexture();
    };

    /// @brief 2D tile map node.
    class TileMap2D : public Node2D {
    private:
        Rune::SpriteBatch* batch;

        std::unordered_map<Vector2i, TileChunk*> chunks;

    public:
        /// @brief Create a tilemap from a source texture
        TileMap2D();

        /// @brief Load a source texture
        void LoadSourceTexture(std::string path);

        /**
         * @brief Registers the Sprite2D class with the ClassDB system.
         * 
         * Used for reflection, serialization, or scripting.
         */
        static void Register();

        /// @brief The size of the tiles
        Vector2i tileSize;

        /// @brief The offset from the top left of the sprite where the sprites start
        Vector2i tileOffset;

        /// @brief Seperation in pixels between the files
        Vector2i tileSeperation;

        /**
         * @brief Set a tile at a position
         * 
         * @param tilePosition The position of the source tile in the tilemap
         * @param destination Where to place the file on the tilemap
         */
        void SetTile(Vector2i tilePosition, Vector2i destination);

        /// @brief Add a chunk to the map
        /// @param position Position in chunk space
        void AddChunk(Vector2i position);

        /// @brief Get a chunk from the map
        /// @param position Position in chunk space
        TileChunk* GetChunk(Vector2i position);

        /**
         * @brief Called every render frame to draw the tilemap.
         * 
         * Override from Node2D.
         */
        void OnRender() override;
    };
}