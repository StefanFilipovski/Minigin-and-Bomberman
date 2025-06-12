#include "DelayedStartScreenLoader.h"
#include "StartScreenLoader.h"
#include <iostream>

namespace dae {
    void DelayedStartScreenLoader::Update(float deltaTime)
    {
        if (m_ShouldLoadStartScreen) {
            m_DelayTimer += deltaTime;
            if (m_DelayTimer >= m_DelayTime) {
                std::cout << "Delayed start screen loading executing..." << std::endl;
                m_ShouldLoadStartScreen = false;

                // Load start screen
                StartScreenLoader loader;
                loader.LoadStartScreen();
            }
        }
    }

    void DelayedStartScreenLoader::TriggerStartScreenLoad()
    {
        std::cout << "Start screen load triggered - will execute in " << m_DelayTime << " seconds" << std::endl;
        m_ShouldLoadStartScreen = true;
        m_DelayTimer = 0.0f;
    }
}