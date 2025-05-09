#include "BlastResponder.h"
#include "GameObject.h"
#include "DestructibleWallResponder.h"

namespace dae {

    BlastResponder::BlastResponder(Segment segment)
        : m_Segment(segment)
    {
    }

    void BlastResponder::OnCollide(GameObject* other)
    {
        if (!other)
            return;

        // If the other object has a destructible‐wall responder, trigger it
        if (auto* dwr = other->GetComponent<DestructibleWallResponder>())
        {
            dwr->OnCollide(other);
        }
        // Static walls will simply block further explosion raycasts
    }

} // namespace dae

