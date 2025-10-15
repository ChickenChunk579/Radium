#include <Radium/Nodes/2D/TileMap2D.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <Radium/PixelScaleUtil.hpp>
#include <Radium/Application.hpp>

// Helper to set a pixel at (x, y) with RGBA color
void SetPixelRGBA(void *surface, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{

}

namespace Radium::Nodes
{

    TileChunk::TileChunk(void *source, Vector2i tileSize, Vector2i tileOffset, Vector2i tileSeperation)
        : texture(nullptr), data(nullptr), tileSize(tileSize), tileOffset(tileOffset), tileSeperation(tileSeperation)
    {
        data = new Vector2i[width * height];

        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                int index = (j * width) + i;
                data[index] = Vector2i(-1, -1);
            }
        }
    }

    void TileChunk::SetTile(Vector2i tilePosition, Vector2i destination)
    {
        int index = destination.y * width + destination.x;
        data[index] = tilePosition;
    }

    void TileChunk::GenerateTexture()
    {
    }

    TileMap2D::TileMap2D()
        : tileSize(16, 16), tileOffset(0, 0), tileSeperation(0, 0)
    {
        std::vector<float> quad = {
            //  x,   y,     u,    v
            0.0f, 1.0f,  0.0f, 1.0f,  // Top Left
            1.0f, 1.0f,  1.0f, 1.0f,  // Top Right
            1.0f, 0.0f,  1.0f, 0.0f,  // Bottom Right

            0.0f, 1.0f,  0.0f, 1.0f,  // Top Left
            1.0f, 0.0f,  1.0f, 0.0f,  // Bottom Right
            0.0f, 0.0f,  0.0f, 0.0f   // Bottom Left
        };

        char data[] = {
            (char)255, (char)255, (char)255, (char)255,
            (char)255, (char)255, (char)255, (char)255,
            (char)255, (char)255, (char)255, (char)255,
            (char)255, (char)255, (char)255, (char)255
        };
        auto tempTexture = new Rune::Texture(1, 1, 4, data, Rune::SamplingMode::Linear);
        batch = new Rune::SpriteBatch(tempTexture, Rune::SpriteOrigin::TopLeft);
    }

    void TileMap2D::LoadSourceTexture(std::string path)
    {
        //sourceSurface = IMG_Load(path.c_str());
    }

    void TileMap2D::Register()
    {
        CLASSDB_REGISTER_SUBCLASS(TileMap2D, Node2D);

        CLASSDB_DECLARE_PROPERTY(TileMap2D, Vector2i, tileSize);
        CLASSDB_DECLARE_PROPERTY(TileMap2D, Vector2i, tileOffset);
        CLASSDB_DECLARE_PROPERTY(TileMap2D, Vector2i, tileSeperation);
    }

    void TileMap2D::SetTile(Vector2i tilePosition, Vector2i destination)
    {
    }

    void TileMap2D::OnRender()
    {
        float scale = Radium::GetPixelScale();
        for (auto pair : chunks) {
            Vector2i pos = pair.first;
            TileChunk* chunk = pair.second;

            Flux::Info("Drawing chunk at {}, {}", pos.x, pos.y);

            Vector2f scaledPosition = Vector2f((TileChunk::width * tileSize.x) * pos.x, (TileChunk::height * tileSize.y) * pos.y);
        
            delete batch;
            
            chunk->GenerateTexture();

            
            // Calculate world position of the chunk
            Vector2f worldPos = Vector2f((TileChunk::width * tileSize.x) * pos.x, (TileChunk::height * tileSize.y) * pos.y);
            
            // Apply camera offset
            Vector2f screenPos = worldPos;
            if (Radium::currentApplication) {
                screenPos = screenPos - Radium::currentApplication->GetCamera()->offset;
            }
            
            float drawX = screenPos.x * scale;
            float drawY = screenPos.y * scale;

            

            batch = new Rune::SpriteBatch(chunk->texture, Rune::SpriteOrigin::TopLeft);
        
            batch->Begin();

            batch->DrawImageRect(
                drawX, drawY,
                TileChunk::width * tileSize.x * scale,  // width scaled
                TileChunk::height * tileSize.y * scale, // height scaled
                1, 1, 1,                                 // color
                0, 0,
                TileChunk::width * tileSize.x,
                TileChunk::height * tileSize.y,
                TileChunk::width * tileSize.x,
                TileChunk::height * tileSize.y,
                0                                       // rotation
            );
        
            batch->End();
        }
    }

    void TileMap2D::AddChunk(Vector2i pos)
    {
        
    }

    TileChunk* TileMap2D::GetChunk(Vector2i pos) {
        auto it = chunks.find(pos);
        if (it != chunks.end()) return it->second;
        return nullptr;

    }
}
