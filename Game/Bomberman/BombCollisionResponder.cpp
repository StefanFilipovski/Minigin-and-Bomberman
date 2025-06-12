#include "BombCollisionResponder.h"
#include "BombComponent.h"
#include "BlastResponder.h"
#include "CollisionComponent.h"
#include "GameObject.h"

namespace dae {

    void BombCollisionResponder::OnCollide(GameObject* other)
    {
        if (!other || !m_Bomb) return;

        // Safety check for deleted objects
        if (!m_Bomb->GetOwner() || m_Bomb->GetOwner()->IsMarkedForDeletion()) return;
        if (other->IsMarkedForDeletion()) return;

        // Check if this bomb is already exploded or marked for cleanup
        if (m_Bomb->IsExploded() || m_Bomb->IsMarkedForCleanup()) return;

        // Check if hit by a blast
        if (auto* cc = other->GetComponent<CollisionComponent>()) {
            if (cc->GetResponder() && dynamic_cast<BlastResponder*>(cc->GetResponder())) {
                // Chain reaction - explode this bomb
                m_Bomb->ForceExplode();
            }
        }
    }
}