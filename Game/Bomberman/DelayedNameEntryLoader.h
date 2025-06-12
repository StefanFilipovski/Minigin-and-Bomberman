#pragma once
#include "Component.h"

namespace dae {
    class DelayedNameEntryLoader : public Component {
    public:
        DelayedNameEntryLoader(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override;
        void TriggerNameEntryLoad();

    private:
        bool m_ShouldLoadNameEntry{ false };
        float m_DelayTimer{ 0.0f };
        float m_DelayTime{ 0.1f }; // 100ms delay
    };
}