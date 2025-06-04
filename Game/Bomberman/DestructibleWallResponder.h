#pragma once
#include "CollisionResponder.h"
#include <vec3.hpp>

namespace dae {
    class GameObject;
    class SpriteSheetComponent;
    class CollisionComponent;
    class PlayerComponent;
    class BlastResponder;

    // A wall that blocks movement until destroyed by a blast
    class DestructibleWallResponder final : public CollisionResponder {
    public:
        explicit DestructibleWallResponder(GameObject* owner);
        ~DestructibleWallResponder() override = default;

        // Called on any overlap: reverts movement or triggers crumble on blasts
        void OnCollide(GameObject* other) override;
        void SpawnPowerUpAt(const glm::vec3& position);
       


    private:
        GameObject* m_pOwner;
        SpriteSheetComponent* m_pSheet{ nullptr };
        CollisionComponent* m_pCollider{ nullptr };
        bool                    m_Started{ false };
    };
}
