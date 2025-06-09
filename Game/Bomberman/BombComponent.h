#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include <string>
#include "Observer.h"
#include "IBombState.h"

namespace dae {
    class GameObject;
    class SpriteSheetComponent;
    class Scene;

    class BombComponent : public Component, public Subject
    {
    public:
        float s_TileSize = 16.f;

        explicit BombComponent(GameObject* owner);
        ~BombComponent() override = default;

        void Init(const std::string& spriteSheet,
            int cols, int rows, float frameTime,
            int range, float fuseTime,
            Scene& scene);

        void Update(float dt) override;

        // called by the states
        void Explode();
        void TransitionTo(IBombState* newState);

        bool IsExploded() const { return m_Exploded; }
        void ForceExplode();

        void MarkForCleanup() { m_MarkedForCleanup = true; }
        bool IsMarkedForCleanup() const { return m_MarkedForCleanup; }

    private:
        // allow these two state‐classes access to private members
        friend struct BombFuseState;
        friend struct BombHideState;

        // --- state machine pointer ---
        std::unique_ptr<IBombState>        m_State{ nullptr };

        // --- explosion config ---
        int                                m_Range{ 1 };
        float                              m_FuseTime{ 0.f };

        // --- runtime timers & flags ---
        float                              m_Timer{ 0.f };
        bool                               m_Exploded{ false };
        bool                               m_IsExploding{ false }; // Add this to prevent double explosion
        bool                               m_HidePending{ false };
        float                              m_HideTimer{ 0.f };
        bool                               m_MarkedForCleanup{ false };

        // --- blast storage & rendering ---
        std::vector<std::shared_ptr<GameObject>> m_Blasts;
        SpriteSheetComponent* m_pSprite{ nullptr };

        // --- where to spawn blasts ---
        Scene* m_pScene{ nullptr };
    };
} // namespace dae