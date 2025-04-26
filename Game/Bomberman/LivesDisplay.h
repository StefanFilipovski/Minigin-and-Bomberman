#pragma once
#include "Component.h"
#include "Observer.h"
#include "GameEvents.h"
#include <iostream>

namespace dae
{
    class LivesDisplay : public Component, public Observer
    {
    public:
       
        LivesDisplay(GameObject* owner, int initialLives)
            : Component(owner)
            , m_Lives(initialLives)
        {
        }

        // Observer callback
        void OnNotify(const Event& event) override
        {
            if (event.id == GameEvents::PLAYER_HIT)
            {
                
            }
            else if (event.id == GameEvents::PLAYER_DIED)
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
