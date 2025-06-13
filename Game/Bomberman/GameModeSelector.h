#pragma once
#include "Component.h"

namespace dae {
    // Forward declaration
    class DelayedCoopGameLoader;

    enum class GameMode {
        Story = 0,
        Coop = 1,
        Versus = 2
    };

    class GameModeSelector : public Component {
    public:
        GameModeSelector(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override;

        GameMode GetSelectedMode() const { return m_SelectedMode; }
        void SetSelectedMode(GameMode mode) { m_SelectedMode = mode; }

        void MoveUp();
        void MoveDown();
        void TriggerSelection();
        void TriggerSelection(DelayedCoopGameLoader* coopLoader);

    private:
        GameMode m_SelectedMode{ GameMode::Story };
        bool m_ShouldLoadMode{ false };
        float m_DelayTimer{ 0.0f };
        float m_DelayTime{ 0.1f };
    };
}