#include "TextComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <SDL_ttf.h>
#include <iostream>

namespace dae {

    TextComponent::TextComponent(GameObject* owner, std::shared_ptr<Font> font, const std::string& text)
        : Component(owner), m_font(font), m_text(text)
    {
        if (!m_font) {
            std::cerr << "Error: Font is null in TextComponent constructor!\n";
            return;
        }
        SetText(m_text);
    }

    TextComponent::~TextComponent()
    {
        // Texture will be automatically cleaned up by shared_ptr
        m_textTexture.reset();
    }

    void TextComponent::SetText(const std::string& text)
    {
        m_text = text;
        m_needsUpdate = true;
        UpdateTexture();
    }

    void TextComponent::UpdateTexture()
    {
        if (!m_needsUpdate) return;

        // Clear old texture first
        m_textTexture.reset();

        if (!m_font || !m_font->GetFont()) {
            std::cerr << "Error: Font is invalid in TextComponent!\n";
            m_needsUpdate = false;
            return;
        }

        // Don't create texture for empty text
        if (m_text.empty()) {
            m_needsUpdate = false;
            return;
        }

        const SDL_Color color = { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), color);
        if (!surf) {
            std::cerr << "Error: TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
            m_needsUpdate = false;
            return;
        }

        auto renderer = Renderer::GetInstance().GetSDLRenderer();
        if (!renderer) {
            std::cerr << "Error: SDL Renderer is null!\n";
            SDL_FreeSurface(surf);
            m_needsUpdate = false;
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);

        if (!texture) {
            std::cerr << "Error: SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
            m_needsUpdate = false;
            return;
        }

        m_textTexture = std::make_shared<Texture2D>(texture);
        m_needsUpdate = false;
    }

    void TextComponent::Render() const
    {
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        if (!m_textTexture) {
            return;  // Nothing to render
        }

        auto* transform = GetOwner()->GetComponent<TransformComponent>();
        if (!transform) {
            std::cerr << "Error: TextComponent has no TransformComponent attached!\n";
            return;
        }

        const glm::vec3& pos = transform->GetWorldPosition();
        Renderer::GetInstance().RenderTexture(*m_textTexture, pos.x, pos.y);
    }

} // namespace dae