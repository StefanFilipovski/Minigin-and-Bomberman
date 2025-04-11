#pragma once
#include "Component.h"

namespace dae
{
    class TextComponent;

    class FPSCounterComponent : public Component
    {
    public:
        explicit FPSCounterComponent(GameObject* owner, TextComponent* fpsTextComponent);

        void Update(float deltaTime) override;

    private:
        TextComponent* m_pFpsText;  
        int m_frameCount = 0;
        float m_elapsedTime = 0.0f;
    };
}
