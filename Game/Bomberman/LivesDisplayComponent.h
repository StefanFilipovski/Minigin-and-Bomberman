#pragma once
#include "Component.h"
#include <memory>

namespace dae {
    class TextComponent;
    class Font;

    class LivesDisplayComponent : public Component {
    public:
        explicit LivesDisplayComponent(GameObject* owner);
        ~LivesDisplayComponent() override;

        void Initialize(); // Call this after adding to GameObject
        void SetLives(int lives);  // Just updates display
        void PerformInit();
        void Update(float deltaTime);

    private:
        TextComponent* m_TextComponent{ nullptr };
        std::shared_ptr<Font> m_Font;
        bool m_NeedsInit{ false };
        bool m_Initialized{ false };
        bool m_NeedsRefresh{ false };
    };
}