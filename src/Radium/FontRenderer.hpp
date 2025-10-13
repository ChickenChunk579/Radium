#include <Rune/SpriteBatch.hpp>
#include <vector>
#include <unordered_map>
#include <SDL_ttf.h>

struct CharacterPosition {
    float x;
    float y;
    float w;
    float h;
    int advance;
    int bearingX;
    int bearingY;
    char c;
};


namespace Radium {
    class FontBatch {
    private:
        Rune::SpriteBatch* batch;
        TTF_Font* font;
        

        std::unordered_map<unsigned char, CharacterPosition> characters;

    public:
        FontBatch(std::string fontPath, int ptSize);

        void DrawCharacter(float x, float y, char c);
        void DrawString(float x, float y, std::string str);
        void Begin();
        void End();
    };
}