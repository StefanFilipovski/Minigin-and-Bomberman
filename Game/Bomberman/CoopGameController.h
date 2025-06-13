#pragma once
#include "Component.h"
#include "Observer.h"

namespace dae {
    class CoopGameController : public Component, public Observer {
    public:
        CoopGameController(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override;
        void OnNotify(const Event& event) override;

    private:
        void SpawnCoopExit();

        bool m_GameEnded{ false };
        float m_EndTimer{ 0.0f };
        float m_DelayTime{ 3.0f }; // Show game over for 3 seconds
        bool m_ExitSpawned{ false };
    };
}