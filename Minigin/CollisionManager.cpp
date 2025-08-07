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
        m_collisionComponents.push_back(comp);
    }

    void CollisionManager::Unregister(CollisionComponent* comp)
    {
        m_collisionComponents.erase(
            std::remove(m_collisionComponents.begin(), m_collisionComponents.end(), comp),
            m_collisionComponents.end()
        );
    }

    void CollisionManager::CheckCollisions()
    {
        // First, clean up any null pointers
        m_collisionComponents.erase(
            std::remove_if(m_collisionComponents.begin(), m_collisionComponents.end(),
                [](CollisionComponent* comp) {
                    return comp == nullptr || comp->IsMarkedForDeletion();
                }),
            m_collisionComponents.end()
        );

        const auto& comps = m_collisionComponents;
        const size_t count = comps.size();

        for (size_t i = 0; i < count; ++i)
        {
            const SDL_Rect rectA = comps[i]->GetBoundingBox();

            for (size_t j = i + 1; j < count; ++j)
            {
                // Check if these layers can collide (if you implemented layers)
                // if (!comps[i]->CanCollideWith(comps[j])) continue;

                const SDL_Rect rectB = comps[j]->GetBoundingBox();

                if (AABBIntersect(rectA, rectB))
                {
                    if (auto* rA = comps[i]->GetResponder())
                        rA->OnCollide(comps[j]->GetOwner());
                    if (auto* rB = comps[j]->GetResponder())
                        rB->OnCollide(comps[i]->GetOwner());
                }
            }
        }
    }

    void CollisionManager::DebugDraw(SDL_Renderer* renderer) const
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Grab the floating‐point camera offset
        const auto camOff = Camera::GetInstance().GetOffset();

        // Convert to integer *once*, using the same rule
        const int offX = static_cast<int>(std::round(camOff.x));
        const int offY = static_cast<int>(std::round(camOff.y));

        for (auto* comp : m_collisionComponents)
        {
            // Get the world‐space box
            SDL_Rect r = comp->GetBoundingBox();

            // Subtract the integer offset
            r.x -= offX;
            r.y -= offY;

            SDL_RenderDrawRect(renderer, &r);
        }
    }

    void CollisionManager::Clear()
    {
        m_collisionComponents.clear();
    }


} // namespace dae
