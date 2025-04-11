#pragma once
#include "Component.h"
#include "TransformComponent.h"

namespace dae
{
    class CharacterMoveComponent : public Component
    {
    public:
        // Constructor takes the owner's TransformComponent by reference.
        CharacterMoveComponent(GameObject* owner, TransformComponent& movementTransformComponent);

        void Update(float deltaTime) override;

        // Setter for rotation center, used if the object has no parent.
        void SetRotationCenter(const glm::vec3& center) { m_RotationCenter = center; }

    private:
        TransformComponent& m_MovementTransform;  // Stored as a reference
        float m_Angle{ 0.f };
        float m_Radius{ 50.f };
        float m_Speed{ 1.f };
        glm::vec3 m_RotationCenter{ 0.f, 0.f, 0.f };
        glm::vec3 m_PivotOffset{ 0.f, 0.f, 0.f };
    };
}