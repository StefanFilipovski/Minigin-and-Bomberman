#include "PowerUpCollisionResponder.h"
#include "PowerUpComponent.h"
#include "PlayerComponent.h"
#include "GameObject.h"

namespace dae {
    void PowerUpCollisionResponder::OnCollide(GameObject* other)
    {
        if (auto* player = other->GetComponent<PlayerComponent>()) {
            m_PowerUp->OnPickup(player);
        }
    }
}