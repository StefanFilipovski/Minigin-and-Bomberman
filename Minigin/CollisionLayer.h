#pragma once
#include <cstdint>

namespace dae {
    enum class CollisionLayer : uint32_t {
        None = 0,
        Platform = 1 << 0,
        Ladder = 1 << 1,
        Player = 1 << 2,
        Enemy = 1 << 3,
        Ingredient = 1 << 4,
        Pepper = 1 << 5
    };

    // Operator overloads for combining layers
    inline CollisionLayer operator|(CollisionLayer a, CollisionLayer b) {
        return static_cast<CollisionLayer>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline uint32_t operator&(uint32_t a, CollisionLayer b) {
        return a & static_cast<uint32_t>(b);
    }
}