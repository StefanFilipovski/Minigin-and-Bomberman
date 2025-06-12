#include "CollisionComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "CollisionManager.h"

namespace dae {

    CollisionComponent::CollisionComponent(GameObject* owner)
        : Component(owner)
    {
        CollisionManager::GetInstance().Register(this);
    }

    CollisionComponent::~CollisionComponent()
    {
        // Clear responder first to prevent any callbacks during destruction
        m_responder.reset();

        // Unregister from collision manager
        CollisionManager::GetInstance().Unregister(this);
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
        // Multiple safety checks
        if (!GetOwner()) {
            return SDL_Rect{ 0, 0, 0, 0 };
        }

        // Check if owner is marked for deletion
        if (GetOwner()->IsMarkedForDeletion()) {
            return SDL_Rect{ 0, 0, 0, 0 };
        }

        // Get transform component safely - use the safe method
        auto* transform = GetOwner()->GetTransformSafe();
        if (!transform) {
            // This object doesn't have a transform - return empty rect
            return SDL_Rect{ 0, 0, 0, 0 };
        }

        try {
            const auto& pos = transform->GetWorldPosition();

            SDL_Rect rect;
            rect.x = static_cast<int>(pos.x + m_offset.x);
            rect.y = static_cast<int>(pos.y + m_offset.y);
            rect.w = static_cast<int>(m_width);
            rect.h = static_cast<int>(m_height);
            return rect;
        }
        catch (...) {
            // Return empty rect if any exception occurs
            return SDL_Rect{ 0, 0, 0, 0 };
        }
    }

    void CollisionComponent::Update(float deltaTime)
    {
        (void)deltaTime;
    }

    void CollisionComponent::SetResponder(std::unique_ptr<CollisionResponder> responder)
    {
        m_responder = std::move(responder);
    }

    
}