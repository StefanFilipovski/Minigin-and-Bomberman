#pragma once
#include "Component.h"
#include <string>
#include <vector>

namespace dae {
    class Scene;
    class SpriteSheetComponent;
    class GameObject;


    // Attach to any GameObject to make it a ticking bomb that
    // after a fuse, explodes in a cross pattern using parent-child.
    class BombComponent final : public Component {
    public:
        static constexpr float s_TileSize = 16.f;

        BombComponent(GameObject* owner);
        ~BombComponent() override = default;

        void Init(const std::string& spriteSheet,
            int cols, int rows, float frameTime,
            int range, float fuseTime,
            Scene& scene);
        void Update(float dt) override;

    private:
        void Explode();

        // explosion parameters
        int    m_Range{};
        float  m_FuseTime{};
        float  m_Timer{};
        bool   m_Exploded{};

        // new: hide-timer for spawned blasts
        bool   m_HidePending{};
        float  m_HideTimer{};
        std::vector<std::shared_ptr<dae::GameObject>> m_Blasts;

        Scene* m_pScene{};
        SpriteSheetComponent* m_pSprite{};
    };
} // namespace dae