#include "CoopExitResponder.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "CoopLevelManager.h"
#include "SoundIds.h"
#include "ServiceLocator.h"
#include <iostream>

namespace dae {
    void CoopExitResponder::OnCollide(GameObject* other)
    {
        // Prevent multiple triggers
        if (m_Triggered) return;

        if (other && other->GetComponent<PlayerComponent>()) {
            m_Triggered = true;

            std::cout << "Co-op player entered exit! Transitioning to next level..." << std::endl;

            ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_LEVEL_COMPLETE, 1.0f);

            // Queue the transition using CoopLevelManager instead of LevelManager
            CoopLevelManager::GetInstance().QueueLevelTransition();
        }
    }
}