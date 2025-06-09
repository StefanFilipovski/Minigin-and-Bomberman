#pragma once
#include "Component.h"
#include <memory>

namespace dae {
    class TextComponent;
    class Font;

    class ScoreComponent : public Component {
    public:
        explicit ScoreComponent(GameObject* owner);

        void Initialize(); // Call this after adding to GameObject
        void SetScore(int score);  // Just updates display

    private:
        TextComponent* m_TextComponent{ nullptr };
        std::shared_ptr<Font> m_Font;
    };
}