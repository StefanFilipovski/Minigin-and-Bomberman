#include "FPSCounter.h"
#include "TextComponent.h"
#include <iostream>

dae::FPSCounterComponent::FPSCounterComponent(GameObject* owner, TextComponent* fpsTextComponent)
    : Component(owner), m_pFpsText(fpsTextComponent)
{
}


void dae::FPSCounterComponent::Update(float deltaTime)
{
    m_frameCount++;
    m_elapsedTime += deltaTime;

    if (m_elapsedTime >= 1.0f)  // Update FPS every second
    {
        if (!m_pFpsText)
        {
            std::cerr << "Error: FPSCounterComponent has no TextComponent assigned!\n";
            return;
        }

        std::string fpsText = "FPS: " + std::to_string(m_frameCount);
        m_pFpsText->SetText(fpsText);

        m_frameCount = 0;
        m_elapsedTime = 0.0f;
    }
}


