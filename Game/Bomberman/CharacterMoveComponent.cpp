#include "CharacterMoveComponent.h"
#include "GameObject.h"
#include <cmath>
#include <SDL_stdinc.h>

dae::CharacterMoveComponent::CharacterMoveComponent(GameObject* owner, TransformComponent& movementTransformComponent)
    : Component(owner), m_MovementTransform(movementTransformComponent)
{
   
    if (!GetOwner()->GetParent())
    {
        m_RotationCenter = m_MovementTransform.GetLocalPosition();
    }
}

void dae::CharacterMoveComponent::Update(float deltaTime)
{
  
    glm::vec3 centerPos;
    if (GetOwner()->GetParent())
    {
        centerPos = glm::vec3(0.f, 0.f, 0.f);
    }
    else
    {
        centerPos = m_RotationCenter;
    }

    // Increment angle normally using deltaTime.
    m_Angle += m_Speed * deltaTime;
    const float TWO_PI = 2.0f * static_cast<float>(M_PI);
    if (m_Angle > TWO_PI)
        m_Angle -= TWO_PI;

    // Compute the new orbit position relative to the center.
    float x = centerPos.x + static_cast<float>(cos(m_Angle)) * m_Radius;
    float y = centerPos.y + static_cast<float>(sin(m_Angle)) * m_Radius;

    // Adjust by the pivot offset to center the sprite if needed.
    x -= m_PivotOffset.x;
    y -= m_PivotOffset.y;

    // Update the transform's local position.
    m_MovementTransform.SetLocalPosition(x, y, 0.0f);
}
