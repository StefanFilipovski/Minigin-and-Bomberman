#include "TransformComponent.h"
#include "GameObject.h"

dae::TransformComponent::TransformComponent(GameObject* owner)
    : Component(owner), m_LocalPosition(0.0f), m_WorldPosition(0.0f), m_PositionIsDirty(true)
{
    // Don't need to check or store owner - base Component class handles it
    // Remove: m_pOwner = owner;  
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
    // Use GetOwner() from base class instead of m_pOwner
    for (auto child : GetOwner()->GetChildren())
    {
        if (child && !child->IsMarkedForDeletion()) {
            // Safe check for transform
            if (auto* childTransform = child->GetComponent<TransformComponent>()) {
                childTransform->MarkPositionDirty();
            }
        }
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
        GameObject* owner = GetOwner();  // Use base class method
        if (!owner)
        {
            std::cerr << "Error: TransformComponent's owner is nullptr! at address " << this << std::endl;
            return;
        }

        GameObject* parent = owner->GetParent();
        if (parent)
        {
            // Get parent transform safely
            if (auto* parentTransform = parent->GetComponent<TransformComponent>()) {
                // World position is parent's world position plus our local position.
                m_WorldPosition = parentTransform->GetWorldPosition() + m_LocalPosition;
                // World scale is parent's world scale multiplied by our local scale.
                m_WorldScale = parentTransform->GetWorldScale() * m_LocalScale;
            }
            else {
                // Parent has no transform - treat as root
                m_WorldScale = m_LocalScale;
                m_WorldPosition = m_LocalPosition;
            }
        }
        else
        {
            m_WorldScale = m_LocalScale;
            m_WorldPosition = m_LocalPosition;
        }

        m_PositionIsDirty = false;
    }
}