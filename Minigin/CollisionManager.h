#pragma once
#include <vector>
#include <algorithm>
#include "Singleton.h"
struct SDL_Renderer;

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

        void DebugDraw(SDL_Renderer* renderer) const;


    private:
        // Private constructor/destructor provided by Singleton.
        friend class Singleton<CollisionManager>;
        CollisionManager() = default;
        ~CollisionManager() = default;

        std::vector<CollisionComponent*> m_collisionComponents;
    };

} 
