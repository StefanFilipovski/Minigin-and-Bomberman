#include "ExitResponder.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "LevelManager.h"
#include <SoundIds.h>
#include <ServiceLocator.h>
#include "CollisionComponent.h"

namespace dae {
    void ExitResponder::OnCollide(GameObject* other)
    {
        // Prevent multiple triggers
        if (m_Triggered) {
            return;
        }

        // Safety checks
        if (!other || !m_Owner) {
            return;
        }

        // Additional safety - check if objects are being deleted
        if (m_Owner->IsMarkedForDeletion() || other->IsMarkedForDeletion()) {
            return;
        }

        // Check if other object has PlayerComponent
        PlayerComponent* player = nullptr;
        try {
            player = other->GetComponent<PlayerComponent>();
        }
        catch (...) {
            return;  // Object might be invalid
        }

        if (player) {
            m_Triggered = true;

            ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_LEVEL_COMPLETE, 1.0f);

            // Disable this collision immediately to prevent further triggers
            if (auto* collision = m_Owner->GetComponent<CollisionComponent>()) {
                collision->SetResponder(nullptr);
            }

            // Queue the transition - don't do it immediately
            LevelManager::GetInstance().QueueLevelTransition();
        }
    }
}