#pragma once
#include "Component.h"
#include <vec3.hpp>
#include <vec2.hpp>



#pragma once
#include "Component.h"
#include <vec3.hpp>
#include <vec2.hpp>

namespace dae
{
    class GameObject;

    class TransformComponent : public Component
    {
    public:
        TransformComponent(GameObject* owner);
        void SetLocalPosition(const glm::vec3& position);
        void SetLocalPosition(float x, float y, float z);
        const glm::vec3& GetLocalPosition() const { return m_LocalPosition; }

        // Scale
        void SetLocalScale(float x, float y) { m_LocalScale = { x, y }; MarkPositionDirty(); }
        glm::vec2 GetLocalScale() const { return m_LocalScale; }
        glm::vec2 GetWorldScale() const { return m_WorldScale; }

        const glm::vec3& GetWorldPosition();
        void UpdateWorldPosition();  // Recalculate world position if dirty
        void MarkPositionDirty();  // Set dirty flag

     

    private:
       

        glm::vec3 m_LocalPosition{};
        glm::vec3 m_WorldPosition{};
        glm::vec2 m_LocalScale{ 1.0f, 1.0f };
        glm::vec2 m_WorldScale{ 1.0f, 1.0f };
        bool m_PositionIsDirty{ true };  // Start dirty to force initial calculation
    };
}
