#pragma once
#include <vector>
#include <algorithm>
#include "Singleton.h"

namespace dae {

    class CollisionComponent;

  
    class CollisionManager final : public Singleton<CollisionManager>
    {
    public:
        // Register and unregister methods.
        void Register(CollisionComponent* comp);
        void Unregister(CollisionComponent* comp);
        // Check collisions among all registered components.
        void CheckCollisions();

    private:
        // Private constructor/destructor provided by Singleton.
        friend class Singleton<CollisionManager>;
        CollisionManager() = default;
        ~CollisionManager() = default;

        std::vector<CollisionComponent*> m_collisionComponents;
    };

} 
