#pragma once
#include "Component.h"
#include "Texture2D.h"

namespace dae
{
    class RenderComponent : public Component
    {
    public:
        explicit RenderComponent(GameObject* owner);

        void SetTexture(const std::string& filename);
        void Render() const override;

       
        void SetScale(float x, float y) { m_Scale = { x, y }; }
        glm::vec2 GetScale() const { return m_Scale; }

    private:
        std::shared_ptr<Texture2D> m_texture;
        glm::vec2 m_Scale{ 1.0f, 1.0f };
    };

}
