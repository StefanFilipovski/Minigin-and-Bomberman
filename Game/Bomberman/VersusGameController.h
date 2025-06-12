#pragma once
#include "Component.h"
#include "Observer.h"

namespace dae {
    class VersusGameController : public Component, public Observer {
    public:
        VersusGameController(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override;
        void OnNotify(const Event& event) override;

    private:
        void ShowWinner(int winner);

        bool m_GameEnded{ false };
        float m_EndTimer{ 0.0f };
        float m_EndDelay{ 3.0f }; // Show winner for 3 seconds
    };
}