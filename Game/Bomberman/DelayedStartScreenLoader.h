#pragma once
#include "Component.h"

namespace dae {
    class DelayedStartScreenLoader : public Component {
    public:
        DelayedStartScreenLoader(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override;
        void TriggerStartScreenLoad();

    private:
        bool m_ShouldLoadStartScreen{ false };
        float m_DelayTimer{ 0.0f };
        float m_DelayTime{ 0.1f }; // 100ms delay
    };
}