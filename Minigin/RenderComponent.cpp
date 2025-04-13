#include "RenderComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TransformComponent.h"
#include "Camera.h"

dae::RenderComponent::RenderComponent(GameObject* owner)
    : Component(owner)  
{
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
    m_texture = dae::ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::RenderComponent::Render() const
{
    if (!m_texture)
        return;

   
    glm::vec3 pos = GetOwner()->GetTransform().GetWorldPosition();

   
    glm::vec3 cameraOffset(dae::Camera::GetInstance().GetOffset(), 0.0f);
    pos -= cameraOffset; 

    int width, height;
    SDL_QueryTexture(m_texture->GetSDLTexture(), nullptr, nullptr, &width, &height);

    // Apply scaling to the texture dimensions.
    float scaledWidth = static_cast<float>(width) * m_Scale.x;
    float scaledHeight = static_cast<float>(height) * m_Scale.y;

    Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y, scaledWidth, scaledHeight);
}


