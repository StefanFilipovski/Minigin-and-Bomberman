#pragma once
#include "CollisionResponder.h"
#include <iostream>
#include "PlayerComponent.h"
#include "GameObject.h"
namespace dae {
    

    class StaticWallResponder : public CollisionResponder {
    public:
        ~StaticWallResponder() override = default;

        // Called when another object collides with this static wall.
        // In a full implementation, this function would typically cause the dynamic
        // object to react (e.g., block its movement).
        void OnCollide(GameObject* other) override {
            // For now, just output to the console.
           
            // Check if the other object has a PlayerComponent
            if (auto* player = other->GetComponent<PlayerComponent>()) {
                player->RevertMove();
                
            }
            // For other types different logic.
        }
    };

}
