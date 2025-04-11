#include "TextComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <SDL_ttf.h>
#include <iostream>

dae::TextComponent::TextComponent(GameObject* owner, std::shared_ptr<Font> font, const std::string& text)
    : Component(owner), m_font(font), m_text(text)
{
    SetText(m_text);
}

void dae::TextComponent::SetText(const std::string& text)
{
    m_text = text;
    m_needsUpdate = true;

    if (!m_font)
    {
        std::cerr << "Error: Font is null in TextComponent!\n";
        return;
    }

    const SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), color);
    if (!surf)
    {
        std::cerr << "Error: TTF_RenderText_Blended failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
    SDL_FreeSurface(surf);
    if (!texture)
    {
        std::cerr << "Error: SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        return;
    }

    m_textTexture = std::make_shared<Texture2D>(texture);
    m_needsUpdate = false;
}

void dae::TextComponent::Render() const
{
    if (!m_textTexture)
    {
        std::cerr << "Error: TextComponent has no texture to render!\n";
        return;
    }

    dae::TransformComponent* transform = GetOwner()->GetComponent<dae::TransformComponent>();
    if (!transform)
    {
        std::cerr << "Error: TextComponent has no TransformComponent attached!\n";
        return;
    }

    const glm::vec3& pos = transform->GetWorldPosition();
    Renderer::GetInstance().RenderTexture(*m_textTexture, pos.x, pos.y);
}
