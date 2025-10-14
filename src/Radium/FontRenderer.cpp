#include <Radium/FontRenderer.hpp>
#include <SDL2/SDL_surface.h>
#include <iostream>
#include <Flux/Flux.hpp>
#include <Radium/PixelScaleUtil.hpp>

bool ttfInitialized = false;

namespace Radium {

    FontBatch::FontBatch(std::string fontName, int ptSize) {
        if (!ttfInitialized) {
            if (TTF_Init() != 0) {
                Flux::Error("Failed to init TTF: {}", TTF_GetError());
                return;
            }
        }
        

        #ifndef __ANDROID__
        font = TTF_OpenFont(("assets/" + fontName).c_str(), ptSize);
        #else
        font = TTF_OpenFont(fontName.c_str(), ptSize);
        #endif
        if (!font) {
            Flux::Error("Failed to load font: {}", TTF_GetError());
            return;
        }

        // Create the font atlas surface
        const int atlasWidth = 896;
        const int atlasHeight = 896;
        SDL_Surface* atlas = SDL_CreateRGBSurfaceWithFormat(0, atlasWidth, atlasHeight, 32, SDL_PIXELFORMAT_RGBA32);
        if (!atlas) {
            Flux::Error("Failed to create atlas surface: {}", SDL_GetError());
            return;
        }

        int x = 0, y = 0;
        int rowHeight = 0;
        const int padding = 2; // Spacing between glyphs

        for (Uint16 c = 0; c <= 255; c++) {

            // Render the glyph to a surface
            SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(font, c, SDL_Color{255, 255, 255, 255});
            if (!glyphSurface) {
                Flux::Error("Failed to render glyph {}: {}", static_cast<char>(c), TTF_GetError());
                continue;
            }

            // Wrap to next row if needed
            if (x + glyphSurface->w > atlasWidth) {
                x = 0;
                y += rowHeight + padding;
                rowHeight = 0;
            }

            // Set destination rect in atlas
            SDL_Rect destRect = {x, y, glyphSurface->w, glyphSurface->h};

            int minx, maxx, miny, maxy, advance;
            if (TTF_GlyphMetrics(font, c, &minx, &maxx, &miny, &maxy, &advance) != 0) {
                Flux::Warn("Failed to get glyph metrics for {}: {}", static_cast<char>(c), TTF_GetError());
                advance = glyphSurface->w; // fallback
            }

            CharacterPosition characterData = {
                static_cast<float>(x),
                static_cast<float>(y),
                static_cast<float>(glyphSurface->w),
                static_cast<float>(glyphSurface->h),
                advance,
                minx,
                maxy,
                static_cast<char>(c)
            };



            characters[static_cast<unsigned char>(c)] = characterData;



            // Blit glyph into atlas
            SDL_BlitSurface(glyphSurface, nullptr, atlas, &destRect);

            // Update positions
            x += glyphSurface->w + padding;
            if (glyphSurface->h > rowHeight)
                rowHeight = glyphSurface->h;

            SDL_FreeSurface(glyphSurface);
        }

        void* copiedPixels = malloc(atlas->h * atlas->pitch);
        memcpy(copiedPixels, atlas->pixels, atlas->h * atlas->pitch);

        Rune::Texture* texture = new Rune::Texture(atlas->w, atlas->h, atlas->pitch, atlas->pixels, Rune::SamplingMode::Linear);

        batch = new Rune::SpriteBatch(texture, Rune::SpriteOrigin::TopLeft);
        if (!batch) {
            Flux::Error("SpriteBatch is null in FontBatch::FontBatch()");
            return;
        }

        Flux::Info("Created SpriteBatch: {}", static_cast<void*>(batch));

        SDL_FreeSurface(atlas);
    }

    void FontBatch::Begin() {
        if (!batch) {
            Flux::Error("SpriteBatch is null in FontBatch::Begin()");
            return;
        }

        batch->Begin();
    }

    void FontBatch::End() {
        if (!batch) {
            Flux::Error("SpriteBatch is null in FontBatch::End()");
            return;
        }

        batch->End();
    }

    void FontBatch::DrawCharacter(float x, float y, char c) {
        CharacterPosition position = this->characters[c];
        float drawX = x + position.bearingX;
        float drawY = y - position.bearingY;
        float scale = Radium::GetPixelScale();
        batch->DrawImageRect(drawX, drawY, position.w * scale, position.h * scale, 1, 1, 1, position.x, position.y, position.w, position.h, 896, 896, 0);
    }

    void FontBatch::DrawString(float x, float y, std::string str) {
        float cursorX = x;

        for (char c : str) {
            CharacterPosition position = this->characters[static_cast<unsigned char>(c)];

            batch->DrawImageRect(cursorX, y, position.w, position.h, 1, 1, 1, position.x, position.y, position.w, position.h, 896, 896, 0);
        
            cursorX += position.advance;
        }
    }

} // namespace Radium
