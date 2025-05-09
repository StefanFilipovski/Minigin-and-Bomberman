#pragma once
#include "CollisionResponder.h"

namespace dae {
    class GameObject;

    // Attached to each explosion tile. On overlap, breaks any destructible wall.
    class BlastResponder final : public CollisionResponder {
    public:
        // Describes which part of the explosion sprite to use
        enum class Segment { Center, Middle, End };

        explicit BlastResponder(Segment segment);
        ~BlastResponder() override = default;

        // Called when this blast‐tile overlaps a wall
        void OnCollide(GameObject* other) override;

    private:
        Segment m_Segment;
    };
}
