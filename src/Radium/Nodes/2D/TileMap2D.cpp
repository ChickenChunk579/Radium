#include <Radium/Nodes/2D/TileMap2D.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <Radium/PixelScaleUtil.hpp>
#include <Radium/Application.hpp>

// Helper to set a pixel at (x, y) with RGBA color
void SetPixelRGBA(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (!surface)
        return;

    // Only lock if necessary
    if (SDL_MUSTLOCK(surface))
    {
        if (SDL_LockSurface(surface) != 0)
            return;
    }

    // Ensure x and y are in bounds
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
    {
        if (SDL_MUSTLOCK(surface))
            SDL_UnlockSurface(surface);
        return;
    }

    // Get the pixel format's RGBA pixel value
    Uint32 pixelColor = SDL_MapRGBA(surface->format, r, g, b, a);

    // Calculate the address of the pixel
    Uint8 *pixelPtr = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32 *)pixelPtr = pixelColor;

    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
}

namespace Radium::Nodes
{

    TileChunk::TileChunk(SDL_Surface *source, Vector2i tileSize, Vector2i tileOffset, Vector2i tileSeperation)
        : sourceSurface(source), texture(nullptr), data(nullptr), tileSize(tileSize), tileOffset(tileOffset), tileSeperation(tileSeperation)
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
        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
            0,
            width * tileSize.x,
            height * tileSize.y,
            32,
            SDL_PIXELFORMAT_RGBA32);

        if (!surface)
            return;

        // Optional: Fill with transparent color
        SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int index = y * width + x;
                Vector2i srcTile = data[index];

                if (srcTile.x < 0 || srcTile.y < 0)
                    continue; // skip empty tiles

                SDL_Rect srcRect;
                srcRect.x = tileOffset.x + srcTile.x * (tileSize.x + tileSeperation.x);
                srcRect.y = tileOffset.y + srcTile.y * (tileSize.y + tileSeperation.y);
                srcRect.w = tileSize.x;
                srcRect.h = tileSize.y;

                SDL_Rect dstRect;
                dstRect.x = x * tileSize.x;
                dstRect.y = y * tileSize.y;
                dstRect.w = tileSize.x;
                dstRect.h = tileSize.y;

                SDL_BlitSurface(sourceSurface, &srcRect, surface, &dstRect);
            }
        }


        // Store the generated surface
        texture = new Rune::Texture(width * tileSize.x, width * tileSize.y, (width * tileSize.x) * 4, surface->pixels, Rune::SamplingMode::Nearest);
        SDL_FreeSurface(surface);
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
        sourceSurface = IMG_Load(path.c_str());
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
        TileChunk *chunk = new TileChunk(sourceSurface, tileSize, tileOffset, tileSeperation);

        chunks[pos] = chunk;
    }

    TileChunk* TileMap2D::GetChunk(Vector2i pos) {
        auto it = chunks.find(pos);
        if (it != chunks.end()) return it->second;
        return nullptr;

    }
}
