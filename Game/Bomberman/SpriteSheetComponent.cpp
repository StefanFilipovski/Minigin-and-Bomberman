#include "SpriteSheetComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "TransformComponent.h"
#include "Camera.h"
#include <algorithm>

namespace dae {

    SpriteSheetComponent::SpriteSheetComponent(GameObject* owner)
        : Component(owner)
        , m_currentState(AnimationState::None)
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
                while (m_elapsedTime >= m_frameDuration && m_currentFrame < m_frames.size() - 1)
                {
                    m_elapsedTime -= m_frameDuration;
                    m_currentFrame++;
                }
            }
        }
    }

    void SpriteSheetComponent::Render() const
    {
        if (!m_texture || m_frames.empty())
            return;

        // Get world position.
        glm::vec3 pos = GetOwner()->GetTransform().GetWorldPosition();
        glm::vec3 cameraOffset(dae::Camera::GetInstance().GetOffset(), 0.0f);
        pos -= cameraOffset;

        SDL_Rect srcRect = m_frames[m_currentFrame];
        float renderWidth = static_cast<float>(srcRect.w) * m_Scale.x;
        float renderHeight = static_cast<float>(srcRect.h) * m_Scale.y;

        // Center the sprite by subtracting half its dimensions.
        pos.x -= renderWidth * 0.5f;
        pos.y -= renderHeight * 0.5f;

        Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y, renderWidth, renderHeight, &srcRect);
    }

    void SpriteSheetComponent::ChangeAnimation(AnimationState newState, int totalRows, int totalColumns, int targetRow, int startColumn, int frameCount, float newFrameDuration, bool loop)
    {
        if (m_currentState == newState)
            return;

        m_currentState = newState;

        if (!m_texture)
            return;

        int textureWidth = 0, textureHeight = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(), nullptr, nullptr, &textureWidth, &textureHeight);
        int frameWidth = textureWidth / totalColumns;
        int frameHeight = textureHeight / totalRows;

        m_frames.clear();
        for (int c = startColumn; c < startColumn + frameCount; ++c)
        {
            SDL_Rect frame = { c * frameWidth, targetRow * frameHeight, frameWidth, frameHeight };
            m_frames.push_back(frame);
        }
        m_frameDuration = newFrameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
        m_Loop = loop;
    }

    void SpriteSheetComponent::SetIdleFrame(AnimationState newState, int totalRows, int totalColumns, int targetRow, int idleColumn)
    {
        if (m_currentState == newState)
            return;

        m_currentState = newState;

        if (!m_texture)
            return;

        int textureWidth = 0, textureHeight = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(), nullptr, nullptr, &textureWidth, &textureHeight);
        int frameWidth = textureWidth / totalColumns;
        int frameHeight = textureHeight / totalRows;

        m_frames.clear();
        SDL_Rect frame = { idleColumn * frameWidth, targetRow * frameHeight, frameWidth, frameHeight };
        m_frames.push_back(frame);

        m_frameDuration = 0.f;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
        m_Loop = false;
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

    void SpriteSheetComponent::SetFrame(int frameIndex)
    {
        if (!m_frames.empty() && frameIndex >= 0 && frameIndex < static_cast<int>(m_frames.size()))
            m_currentFrame = static_cast<unsigned int>(frameIndex);
    }

    // --- New movement-key handling functions ---

    void SpriteSheetComponent::OnMovementKeyPressed(SDL_Scancode key)
    {
        // Add the key if not already present.
        if (std::find(m_MovementKeys.begin(), m_MovementKeys.end(), key) == m_MovementKeys.end())
            m_MovementKeys.push_back(key);
        UpdateMovementAnimation();
    }

    void SpriteSheetComponent::OnMovementKeyReleased(SDL_Scancode key)
    {
        auto it = std::find(m_MovementKeys.begin(), m_MovementKeys.end(), key);
        if (it != m_MovementKeys.end())
            m_MovementKeys.erase(it);
        UpdateMovementAnimation();
    }

    void SpriteSheetComponent::UpdateMovementAnimation()
    {
        // If there are movement keys still held, use the last one pressed.
        if (!m_MovementKeys.empty())
        {
            SDL_Scancode key = m_MovementKeys.back();

            // Map key to the proper animation.
            if (key == SDL_SCANCODE_UP)
            {
                ChangeAnimation(AnimationState::MoveUp, 4, 3, 0, 0, 3, 0.15f, true);
            }
            else if (key == SDL_SCANCODE_RIGHT)
            {
                ChangeAnimation(AnimationState::MoveRight, 4, 3, 1, 0, 3, 0.15f, true);
            }
            else if (key == SDL_SCANCODE_DOWN)
            {
                ChangeAnimation(AnimationState::MoveDown, 4, 3, 2, 0, 3, 0.15f, true);
            }
            else if (key == SDL_SCANCODE_LEFT)
            {
                ChangeAnimation(AnimationState::MoveLeft, 4, 3, 3, 0, 3, 0.15f, true);
            }
        }
        else
        {
            // No movement key is held: revert to idle.
            SetIdleFrame(AnimationState::Idle, 4, 3, 2, 1);
        }
    }

} // namespace dae
