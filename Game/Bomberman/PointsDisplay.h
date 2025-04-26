#pragma once
#include "Observer.h"
#include "TextComponent.h"
#include "GameEvents.h" 
#include <sstream>
#include <iostream>

namespace dae {

    class PointsDisplay : public Observer {
    public:
        PointsDisplay(int initialPoints, TextComponent* textComponent)
            : m_Points(initialPoints), m_TextComponent(textComponent) {
            UpdateDisplay();
        }

        void OnNotify(const Event& event) override {
            if (event.id == GameEvents::POINTS_ADDED) {
                m_Points += 10;
                UpdateDisplay();
            }
        }

        int GetPoints() const { return m_Points; }

    private:
        void UpdateDisplay() {
            if (m_TextComponent) {
                std::stringstream ss;
                ss << "Score: " << m_Points;
                m_TextComponent->SetText(ss.str());
            }
            std::cout << "Score updated: " << m_Points << std::endl;
        }

        int m_Points;
        TextComponent* m_TextComponent;
    };

}
