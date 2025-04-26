#include "CollisionComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "CollisionManager.h"

namespace dae {

    CollisionComponent::CollisionComponent(GameObject* owner)
        : Component(owner)
    {
        // might want to initialize size from the RenderComponent
        // or from some default value.

        dae::CollisionManager::GetInstance().Register(this);
    }

    void CollisionComponent::SetSize(float width, float height)
    {
        m_width = width;
        m_height = height;
    }

    void CollisionComponent::SetOffset(float offsetX, float offsetY)
    {
        m_offset.x = offsetX;
        m_offset.y = offsetY;
    }

    SDL_Rect CollisionComponent::GetBoundingBox() const
    {
        // We assume that the owner has a TransformComponent.
        // The TransformComponent provides the object's world position.
        const auto& pos = GetOwner()->GetTransform().GetWorldPosition();

        // Create an SDL_Rect for the collision box.
        SDL_Rect rect;
        rect.x = static_cast<int>(pos.x + m_offset.x);
        rect.y = static_cast<int>(pos.y + m_offset.y);
        rect.w = static_cast<int>(m_width);
        rect.h = static_cast<int>(m_height);
        return rect;
    }

    void CollisionComponent::Update(float deltaTime)
    {
        (void)deltaTime;
        // In this example the collision data (size and offset) remain constant.
        // If needed update the collision box here based on dynamic properties.
    }
    void CollisionComponent::SetResponder(std::unique_ptr<CollisionResponder> responder)
    {
        m_responder = std::move(responder);
    }
}
