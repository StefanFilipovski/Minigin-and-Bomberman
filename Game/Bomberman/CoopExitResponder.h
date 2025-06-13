#pragma once
#include "CollisionResponder.h"

namespace dae {
    class GameObject;

    class CoopExitResponder : public CollisionResponder {
    public:
        explicit CoopExitResponder(GameObject* owner) : m_Owner(owner) {}
        void OnCollide(GameObject* other) override;

    private:
        GameObject* m_Owner;
        bool m_Triggered{ false };
    };
}