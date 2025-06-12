#include "CollisionManager.h"
#include "CollisionComponent.h"
#include "CollisionUtilities.h"  
#include <SDL.h>
#include <iostream>
#include "CollisionResponder.h"
#include "Camera.h"

namespace dae {

    void CollisionManager::Register(CollisionComponent* comp)
    {
        if (comp && std::find(m_collisionComponents.begin(), m_collisionComponents.end(), comp) == m_collisionComponents.end()) {
            m_collisionComponents.push_back(comp);
        }
    }

    void CollisionManager::Unregister(CollisionComponent* comp)
    {
        // Mark for deferred removal if we're currently checking collisions
        if (m_IsCheckingCollisions) {
            m_componentsToRemove.push_back(comp);
        }
        else {
            m_collisionComponents.erase(
                std::remove(m_collisionComponents.begin(), m_collisionComponents.end(), comp),
                m_collisionComponents.end()
            );
        }
    }

    void CollisionManager::CheckCollisions()
    {
        m_IsCheckingCollisions = true;

        // Create a copy to avoid issues with components being removed during iteration
        std::vector<CollisionComponent*> componentsCopy = m_collisionComponents;

        const size_t count = componentsCopy.size();
        for (size_t i = 0; i < count; ++i)
        {
            auto* compA = componentsCopy[i];
            if (!compA || !compA->GetOwner()) {
                continue;
            }

            // Compute A's box once
            SDL_Rect rectA;
            try {
                rectA = compA->GetBoundingBox();
            }
            catch (...) {
                continue; // Skip if we can't get bounding box
            }

            for (size_t j = i + 1; j < count; ++j)
            {
                auto* compB = componentsCopy[j];
                if (!compB || !compB->GetOwner()) {
                    continue;
                }

                SDL_Rect rectB;
                try {
                    rectB = compB->GetBoundingBox();
                }
                catch (...) {
                    continue; // Skip if we can't get bounding box
                }

                if (AABBIntersect(rectA, rectB))
                {
                    // Notify both responders (if present)
                    try {
                        if (auto* rA = compA->GetResponder()) {
                            rA->OnCollide(compB->GetOwner());
                        }
                    }
                    catch (...) {
                        // Ignore exceptions from responders
                    }

                    try {
                        if (auto* rB = compB->GetResponder()) {
                            rB->OnCollide(compA->GetOwner());
                        }
                    }
                    catch (...) {
                        // Ignore exceptions from responders
                    }
                }
            }
        }

        m_IsCheckingCollisions = false;

        // Process deferred removals
        for (auto* comp : m_componentsToRemove) {
            m_collisionComponents.erase(
                std::remove(m_collisionComponents.begin(), m_collisionComponents.end(), comp),
                m_collisionComponents.end()
            );
        }
        m_componentsToRemove.clear();
    }

    void CollisionManager::DebugDraw(SDL_Renderer* renderer) const
    {
        if (!renderer) {
            return;
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Grab the floating‐point camera offset
        const auto camOff = Camera::GetInstance().GetOffset();

        // Convert to integer *once*, using the same rule
        const int offX = static_cast<int>(std::round(camOff.x));
        const int offY = static_cast<int>(std::round(camOff.y));

        for (auto* comp : m_collisionComponents)
        {
            if (!comp || !comp->GetOwner()) {
                continue;
            }

            try {
                // Get the world‐space box
                SDL_Rect r = comp->GetBoundingBox();

                // Subtract the integer offset
                r.x -= offX;
                r.y -= offY;

                SDL_RenderDrawRect(renderer, &r);
            }
            catch (...) {
                // Skip drawing if there's any issue
            }
        }
    }

    void CollisionManager::Clear()
    {
        m_IsCheckingCollisions = false;
        m_componentsToRemove.clear();
        m_collisionComponents.clear();
    }

} // namespace dae