#include "Player2CollisionResponder.h"
#include "Player2BalloonComponent.h"
#include "GameObject.h"
#include "PlayerComponent.h"
#include "BaseEnemyComponent.h"
#include "BlastResponder.h"
#include "BombCollisionResponder.h"
#include "CollisionComponent.h"
#include <iostream>

namespace dae {
    Player2CollisionResponder::Player2CollisionResponder(Player2BalloonComponent* player2)
        : m_Player2(player2)
    {
    }

    void Player2CollisionResponder::OnCollide(GameObject* other)
    {
        if (!m_Player2 || m_Player2->IsDead()) return;

        // Check if the other object has a PlayerComponent
        if (auto* player = other->GetComponent<PlayerComponent>()) {
            
            if (player->IsOutOfLives()) {
                return; 
            }

           
            player->TakeDamage(1);
            return;
        }

        // Check for blast/explosion damage (from bomb explosions)
        if (auto* cc = other->GetComponent<CollisionComponent>()) {
            if (auto* blastResponder = dynamic_cast<BlastResponder*>(cc->GetResponder())) {
               
                m_Player2->TakeDamage(1);
                return;
            }

            // Allow walking through bombs - don't revert movement for bomb collisions
            if (dynamic_cast<BombCollisionResponder*>(cc->GetResponder())) {
                return; // Do nothing - let player walk through bombs
            }
        }

        // For walls and other objects, revert movement
        m_Player2->RevertMove();
    }
}