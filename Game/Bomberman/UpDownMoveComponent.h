#pragma once
#include "Component.h"
#include "TransformComponent.h"

namespace dae
{
    class UpDownMoveComponent : public Component
    {
    public:
        UpDownMoveComponent(GameObject* owner, TransformComponent& transform, float speed, float range);
        void Update(float deltaTime) override;

    private:
        TransformComponent& m_Transform;  // Reference to avoid searching each frame
        float m_StartY;
        float m_Speed;
        float m_Range;
        float m_Time;
    };
}
