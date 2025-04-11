#pragma once
#include "Observer.h"
#include <iostream>
#include "GameEvents.h" // Include the game-specific event definitions

namespace dae
{
    class LivesDisplay : public Observer
    {
    public:
        LivesDisplay(int initialLives)
            : m_Lives(initialLives) {
        }

        void OnNotify(const Event& event) override
        {
            if (event.id == GameEvents::PLAYER_DIED)
            {
                m_Lives--;
                UpdateDisplay();
            }
        }

    private:
        int m_Lives;
        void UpdateDisplay()
        {
            std::cout << "Remaining Lives: " << m_Lives << std::endl;
        }
    };
}
