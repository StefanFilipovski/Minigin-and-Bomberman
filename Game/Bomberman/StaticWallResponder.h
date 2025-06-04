#pragma once
#include "CollisionResponder.h"
#include <iostream>
#include "PlayerComponent.h"
#include "GameObject.h"
#include "BaseEnemyComponent.h"

namespace dae {

    class StaticWallResponder : public CollisionResponder {
    public:
        ~StaticWallResponder() override = default;

        void OnCollide(GameObject* other) override {
            // Check if the other object has a PlayerComponent
            if (auto* player = other->GetComponent<PlayerComponent>()) {
                player->RevertMove();
                return;
            }

            // Check for ANY enemy derived from BaseEnemyComponent
            if (auto* enemy = other->GetComponent<BaseEnemyComponent>()) {
                enemy->RevertMove();
                return;
            }

            // For other types different logic.
        }
    };
}