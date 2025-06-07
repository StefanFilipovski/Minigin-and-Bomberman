#include "ExitResponder.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "LevelManager.h"
#include <SoundIds.h>
#include <ServiceLocator.h>

namespace dae {
    void ExitResponder::OnCollide(GameObject* other)
    {
        // Prevent multiple triggers
        if (m_Triggered) return;

        if (other && other->GetComponent<PlayerComponent>()) {
            m_Triggered = true;

            ServiceLocator::GetSoundSystem().Play(SOUND_LEVEL_COMPLETE, 1.0f);

            // Queue the transition instead of doing it immediately
            LevelManager::GetInstance().QueueLevelTransition();
        }
    }
}