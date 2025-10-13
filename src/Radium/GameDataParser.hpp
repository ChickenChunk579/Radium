#include <Radium/json.hpp>
#include <string>
#include <Rune/Rune.hpp>
#include <vector>
#include <Rune/SpriteBatch.hpp>

using json = nlohmann::json;

namespace Radium {
    // SpriteBatch struct
    struct SpriteBatch {
        std::string tag;
        std::string path;
        Rune::SpriteOrigin origin;
    };

    // GameConfig struct
    struct GameConfig {
        std::string title;
        std::vector<int> preferedSize;
        std::vector<int> gravity;
        std::string initialScene;
        std::vector<SpriteBatch> spriteBatches;
    };

    // Serialization for SpriteOrigin
    inline void to_json(json& j, const Rune::SpriteOrigin& origin) {
        j = static_cast<int>(origin);
    }

    inline void from_json(const json& j, Rune::SpriteOrigin& origin) {
        int value = j.get<int>();
        switch (value) {
            case 0: origin = Rune::SpriteOrigin::TopLeft; break;
            case 1: origin = Rune::SpriteOrigin::Center; break;
            default: throw std::invalid_argument("Invalid SpriteOrigin value");
        }
    }

    // Serialization for SpriteBatch
    inline void to_json(json& j, const SpriteBatch& sb) {
        j = json{
            {"tag", sb.tag},
            {"path", sb.path},
            {"origin", sb.origin}
        };
    }

    inline void from_json(const json& j, SpriteBatch& sb) {
        j.at("tag").get_to(sb.tag);
        j.at("path").get_to(sb.path);
        j.at("origin").get_to(sb.origin);
    }

    // Serialization for GameConfig
    inline void to_json(json& j, const GameConfig& config) {
        j = json{
            {"title", config.title},
            {"preferedSize", config.preferedSize},
            {"gravity", config.gravity},
            {"initialScene", config.initialScene},
            {"spriteBatches", config.spriteBatches}
        };
    }

    inline void from_json(const json& j, GameConfig& config) {
        j.at("title").get_to(config.title);
        j.at("preferedSize").get_to(config.preferedSize);
        j.at("gravity").get_to(config.gravity);
        j.at("initialScene").get_to(config.initialScene);
        j.at("spriteBatches").get_to(config.spriteBatches);
    }
};