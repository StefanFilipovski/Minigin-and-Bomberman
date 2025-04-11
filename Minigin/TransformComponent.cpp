#include "TransformComponent.h"
#include "GameObject.h"

dae::TransformComponent::TransformComponent(GameObject* owner)
    : Component(owner), m_LocalPosition(0.0f), m_WorldPosition(0.0f), m_PositionIsDirty(true)
{
    if (!owner)
    {
        throw std::runtime_error("TransformComponent initialized with nullptr owner!");
    }

    m_pOwner = owner;  
    
}

void dae::TransformComponent::SetLocalPosition(const glm::vec3& position)
{
    m_LocalPosition = position;
    MarkPositionDirty();  
  
}

void dae::TransformComponent::SetLocalPosition(float x, float y, float z)
{
    m_LocalPosition = { x, y, z };
    MarkPositionDirty();  
    
}


void dae::TransformComponent::MarkPositionDirty()
{
    m_PositionIsDirty = true;
    // Recursively mark all children's transforms as dirty
    for (auto child : m_pOwner->GetChildren())
    {
        // Assuming every GameObject has a TransformComponent:
        child->GetTransform().MarkPositionDirty();
    }
}

const glm::vec3& dae::TransformComponent::GetWorldPosition()
{
    if (m_PositionIsDirty)
    {
        UpdateWorldPosition();
    }
    return m_WorldPosition;
}

void dae::TransformComponent::UpdateWorldPosition()
{
    if (m_PositionIsDirty)
    {
        if (!m_pOwner)
        {
            std::cerr << "Error: TransformComponent's owner is nullptr! at address " << this << std::endl;
            return;
        }

        GameObject* parent = m_pOwner->GetParent();
        if (parent)
        {
            
            // World position is parent's world position plus our local position.
            m_WorldPosition = parent->GetTransform().GetWorldPosition() + m_LocalPosition;
            // World scale is parent's world scale multiplied by our local scale.
            m_WorldScale = parent->GetTransform().GetWorldScale() * m_LocalScale;
        }
        else
        {
            m_WorldScale = m_LocalScale;
            m_WorldPosition = m_LocalPosition;
        }

        m_PositionIsDirty = false;
    }
}


