#include "BombCollisionResponder.h"
#include "BombComponent.h"
#include "BlastResponder.h"
#include "CollisionComponent.h"
#include "GameObject.h"

namespace dae {

    void BombCollisionResponder::OnCollide(GameObject* other)
    {
        if (!other || !m_Bomb) return;

        // Check if this bomb is already exploded or marked for cleanup
        if (m_Bomb->IsExploded() || m_Bomb->IsMarkedForCleanup()) return;

        // Check if hit by a blast
        if (auto* cc = other->GetComponent<CollisionComponent>()) {
            if (dynamic_cast<BlastResponder*>(cc->GetResponder())) {
                // Add a tiny delay to prevent simultaneous explosions
                // This helps with the race condition
                m_Bomb->ForceExplode();
            }
        }
    }
}