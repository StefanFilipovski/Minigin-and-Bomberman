#pragma once
#include "Component.h"
#include "CoopLevelManager.h"
#include "ScoreManager.h"
#include <iostream>

namespace dae {
    class DelayedCoopGameLoader : public Component {
    public:
        DelayedCoopGameLoader(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override;
        void TriggerCoopGameLoad();

    private:
        bool m_ShouldLoadCoopGame{ false };
        float m_DelayTimer{ 0.0f };
        float m_DelayTime{ 0.1f }; // 100ms delay
    };
}