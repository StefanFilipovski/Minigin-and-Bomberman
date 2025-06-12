#pragma once
#include "Component.h"
#include "Observer.h"
#include <memory>

namespace dae {
    class TextComponent;
    class Font;

    class LivesDisplayComponent : public Component, public Observer {
    public:
        explicit LivesDisplayComponent(GameObject* owner, int initialLives = 3);

        void Initialize(); // Call this after adding to GameObject
        void OnNotify(const Event& event) override;

        void SetLives(int lives);
        int GetLives() const { return m_CurrentLives; }
        void ResetToMaxLives() { SetLives(m_MaxLives); }

    private:
        void UpdateDisplay();

        TextComponent* m_TextComponent{ nullptr };
        std::shared_ptr<Font> m_Font;
        int m_CurrentLives;
        int m_MaxLives;
    };
}