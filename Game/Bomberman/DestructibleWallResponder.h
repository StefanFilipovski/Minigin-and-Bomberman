#pragma once
#include "CollisionResponder.h"
#include "Scene.h"
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
        explicit DestructibleWallResponder(GameObject* owner, Scene* scene);
        ~DestructibleWallResponder() override = default;

        void OnCollide(GameObject* other) override;
     

    private:
        GameObject* m_pOwner;
        Scene* m_pScene;  
        SpriteSheetComponent* m_pSheet{ nullptr };
        CollisionComponent* m_pCollider{ nullptr };
        bool m_Started{ false };

        glm::vec3 m_SpawnPosition{};
        bool m_ShouldSpawnPowerUp{ false };
    };
}
