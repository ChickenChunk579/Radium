#pragma once
#include <random>

namespace Radium {
    namespace Random {

        // Internal engine (seeded once)
        inline std::mt19937& GetEngine() {
            static std::random_device rd;
            static std::mt19937 engine(rd());
            return engine;
        }

        /// @brief Returns a random integer between [min, max] inclusive
        inline int IntBetween(int min, int max) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(GetEngine());
        }

        /// @brief Returns a random float between [min, max]
        inline float FloatBetween(float min, float max) {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(GetEngine());
        }
    }
}
