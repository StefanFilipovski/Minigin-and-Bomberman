#include "UpDownMoveComponent.h"
#include <cmath>

dae::UpDownMoveComponent::UpDownMoveComponent(GameObject* owner, TransformComponent& transform, float speed, float range)
    : Component(owner), m_Transform(transform), m_Speed(speed), m_Range(range), m_Time(0.0f)
{
    m_StartY = m_Transform.GetLocalPosition().y;
}

void dae::UpDownMoveComponent::Update(float deltaTime)
{
    
    m_Time += deltaTime;
    float newY = m_StartY + sin(m_Time * m_Speed) * m_Range;

    glm::vec3 currentPos = m_Transform.GetLocalPosition();

    
    m_Transform.SetLocalPosition(currentPos.x, newY, currentPos.z);
}
