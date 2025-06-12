#include "DelayedNameEntryLoader.h"
#include "NameEntryScreenLoader.h"
#include "PlayerNameManager.h"
#include <iostream>

namespace dae {
    void DelayedNameEntryLoader::Update(float deltaTime)
    {
        if (m_ShouldLoadNameEntry) {
            m_DelayTimer += deltaTime;
            if (m_DelayTimer >= m_DelayTime) {
                std::cout << "Delayed name entry loading executing..." << std::endl;
                m_ShouldLoadNameEntry = false;

                // Clear any existing name
                PlayerNameManager::GetInstance().ClearName();

                // Load name entry screen
                NameEntryScreenLoader loader;
                loader.LoadNameEntryScreen();
            }
        }
    }

    void DelayedNameEntryLoader::TriggerNameEntryLoad()
    {
        std::cout << "Name entry load triggered - will execute in " << m_DelayTime << " seconds" << std::endl;
        m_ShouldLoadNameEntry = true;
        m_DelayTimer = 0.0f;
    }
}