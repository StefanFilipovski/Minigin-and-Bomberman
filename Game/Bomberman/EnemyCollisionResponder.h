#pragma once
#include "CollisionResponder.h"
#include "PlayerComponent.h"
#include "BalloonComponent.h"
#include "GameObject.h"

namespace dae {
    /// Handles collisions for AI enemies: player contact only
    class EnemyCollisionResponder final : public CollisionResponder {
    public:
        explicit EnemyCollisionResponder(BalloonComponent* balloon)
            : m_Balloon(balloon)
            
        {
        }

        void OnCollide(GameObject* other) override
        {
            // Player collision → deal damage once, then ignore further hits
           
                if (auto* player = other->GetComponent<PlayerComponent>())
                {
                    player->TakeDamage(1);
                   
                }
            
        }

    private:
        BalloonComponent* m_Balloon;
   
    };
}
