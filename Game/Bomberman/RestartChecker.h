#pragma once
#include "Component.h"

namespace dae {
    class RestartChecker : public Component {
    public:
        RestartChecker(GameObject* owner) : Component(owner) {}
        void Update(float deltaTime) override;

    private:
        static bool s_RestartRequested;
        float m_DelayTimer{ 0.0f };
        bool m_RestartTriggered{ false };
    };
}