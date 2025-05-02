#include "CollisionManager.h"
#include "CollisionComponent.h"
#include "CollisionUtilities.h"  
#include <SDL.h>
#include <iostream>
#include "CollisionResponder.h"

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
        const auto& comps = m_collisionComponents;
        const size_t count = comps.size();
        for (size_t i = 0; i < count; ++i)
        {
            // Compute A’s box once
            const SDL_Rect rectA = comps[i]->GetBoundingBox();

            for (size_t j = i + 1; j < count; ++j)
            {
                const SDL_Rect rectB = comps[j]->GetBoundingBox();

                if (AABBIntersect(rectA, rectB))
                {
                    // Notify both responders (if present)
                    if (auto* rA = comps[i]->GetResponder())
                        rA->OnCollide(comps[j]->GetOwner());
                    if (auto* rB = comps[j]->GetResponder())
                        rB->OnCollide(comps[i]->GetOwner());
                }
            }
        }
    }

    void CollisionManager::DebugDraw(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto* comp : m_collisionComponents) {
            SDL_Rect r = comp->GetBoundingBox();
            SDL_RenderDrawRect(renderer, &r);
        }
    }




} // namespace dae
