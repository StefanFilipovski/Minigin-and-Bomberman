#include "SpriteSheetComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TransformComponent.h"

namespace dae {

    SpriteSheetComponent::SpriteSheetComponent(GameObject* owner)
        : Component(owner)
    {
    }

    void SpriteSheetComponent::SetSpriteSheet(const std::string& filename, const std::vector<SDL_Rect>& frames, float frameDuration)
    {
        m_texture = ResourceManager::GetInstance().LoadTexture(filename);
        m_frames = frames;
        m_frameDuration = frameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }

    void SpriteSheetComponent::SetSpriteSheet(const std::string& filename, int totalRows, int totalColumns, int targetRow, float frameDuration)
    {
        m_texture = ResourceManager::GetInstance().LoadTexture(filename);

        // Query texture dimensions.
        int textureWidth = 0, textureHeight = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(), nullptr, nullptr, &textureWidth, &textureHeight);

        int frameWidth = textureWidth / totalColumns;
        int frameHeight = textureHeight / totalRows;
        m_frames.clear();
        for (int c = 0; c < totalColumns; ++c)
        {
            SDL_Rect frame = { c * frameWidth, targetRow * frameHeight, frameWidth, frameHeight };
            m_frames.push_back(frame);
        }

        m_frameDuration = frameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }

    void SpriteSheetComponent::Update(float deltaTime)
    {
        if (m_frameDuration > 0.f && !m_frames.empty())
        {
            m_elapsedTime += deltaTime;
            if (m_Loop)
            {
                while (m_elapsedTime >= m_frameDuration)
                {
                    m_elapsedTime -= m_frameDuration;
                    m_currentFrame = (m_currentFrame + 1) % m_frames.size();
                }
            }
            else
            {
                // Non-looping: only advance until the last frame.
                while (m_elapsedTime >= m_frameDuration && m_currentFrame < m_frames.size() - 1)
                {
                    m_elapsedTime -= m_frameDuration;
                    m_currentFrame++;
                }
                // Optionally, clamp m_elapsedTime if needed.
            }
        }
    }

    void SpriteSheetComponent::Render() const
    {
        if (!m_texture || m_frames.empty())
            return;

        auto& pos = GetOwner()->GetTransform().GetWorldPosition();
        const SDL_Rect& srcRect = m_frames[m_currentFrame];

        float renderWidth = static_cast<float>(srcRect.w) * m_Scale.x;
        float renderHeight = static_cast<float>(srcRect.h) * m_Scale.y;

        Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y, renderWidth, renderHeight, &srcRect);
    }

    void SpriteSheetComponent::ChangeAnimationRow(int totalRows, int totalColumns, int targetRow, float newFrameDuration)
    {
        if (!m_texture)
            return;

        int textureWidth = 0, textureHeight = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(), nullptr, nullptr, &textureWidth, &textureHeight);

        int frameWidth = textureWidth / totalColumns;
        int frameHeight = textureHeight / totalRows;

        m_frames.clear();
        for (int c = 0; c < totalColumns; ++c)
        {
            SDL_Rect frame = { c * frameWidth, targetRow * frameHeight, frameWidth, frameHeight };
            m_frames.push_back(frame);
        }
        m_frameDuration = newFrameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }
}
