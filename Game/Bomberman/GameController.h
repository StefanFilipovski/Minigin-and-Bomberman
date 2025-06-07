#pragma once
#include "Component.h"

namespace dae {
    class GameController : public Component {
    public:
        GameController(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override;
    };
}