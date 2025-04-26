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

    void dae::CollisionManager::CheckCollisions()
    {
        /*std::cout << "CheckCollisions called" << std::endl;*/
        //std::cout << "Number of collision components: " << m_collisionComponents.size() << std::endl;
        const size_t count = m_collisionComponents.size();
        for (size_t i = 0; i < count; ++i)
        {
            SDL_Rect rectA = m_collisionComponents[i]->GetBoundingBox();
            for (size_t j = i + 1; j < count; ++j)
            {
                SDL_Rect rectB = m_collisionComponents[j]->GetBoundingBox();
                // Print bounding box info for debugging
                std::cout << "Checking collision between components " << i << " and " << j << std::endl;
                std::cout << "Component " << i << " bbox: (" << rectA.x << ", " << rectA.y << ", "
                    << rectA.w << ", " << rectA.h << ")" << std::endl;
                std::cout << "Component " << j << " bbox: (" << rectB.x << ", " << rectB.y << ", "
                    << rectB.w << ", " << rectB.h << ")" << std::endl;

                if (dae::AABBIntersect(rectA, rectB))
                {
                    std::cout << "Collision detected between component " << i << " and " << j << std::endl;
                    if (m_collisionComponents[i]->GetResponder())
                        m_collisionComponents[i]->GetResponder()->OnCollide(m_collisionComponents[j]->GetOwner());
                    if (m_collisionComponents[j]->GetResponder())
                        m_collisionComponents[j]->GetResponder()->OnCollide(m_collisionComponents[i]->GetOwner());
                }
                else
                {
                    std::cout << "No collision between component " << i << " and " << j << std::endl;
                }
            }
        }
    }



} // namespace dae
