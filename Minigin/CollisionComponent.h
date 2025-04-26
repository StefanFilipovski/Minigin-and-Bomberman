#pragma once
#include "Component.h"
#include <SDL.h>
#include <vec2.hpp>
#include <memory>  
#include "CollisionResponder.h"



namespace dae {

    class CollisionComponent : public Component {
    public:
        explicit CollisionComponent(GameObject* owner);
        virtual ~CollisionComponent() = default;

        void SetSize(float width, float height);
        void SetOffset(float offsetX, float offsetY);
        SDL_Rect GetBoundingBox() const;
        void Update(float deltaTime) override;

        
        void SetResponder(std::unique_ptr<CollisionResponder> responder);

        // Return a raw pointer (non-owning) for accessing the responder externally.
        CollisionResponder* GetResponder() const { return m_responder.get(); }

    private:
        float m_width{ 0.f };
        float m_height{ 0.f };
        glm::vec2 m_offset{ 0.f, 0.f };
        std::unique_ptr<CollisionResponder> m_responder{ nullptr };
    };

} 
