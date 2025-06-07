#pragma once
#include "CollisionResponder.h"

namespace dae {
    class GameObject;

    class ExitResponder : public CollisionResponder {
    public:
        explicit ExitResponder(GameObject* owner) : m_Owner(owner) {}
        void OnCollide(GameObject* other) override;

    private:
        GameObject* m_Owner;
        bool m_Triggered{ false };
    };
}