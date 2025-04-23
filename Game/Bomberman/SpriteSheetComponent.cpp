#include "SpriteSheetComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "TransformComponent.h"
#include "Camera.h"

namespace dae {

    SpriteSheetComponent::SpriteSheetComponent(GameObject* owner)
        : Component(owner)
        , m_currentState(AnimationState::None)
    {
    }

    void SpriteSheetComponent::SetSpriteSheet(const std::string& filename,
        const std::vector<SDL_Rect>& frames,
        float frameDuration)
    {
        m_texture = ResourceManager::GetInstance().LoadTexture(filename);
        m_frames = frames;
        m_frameDuration = frameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }

    void SpriteSheetComponent::SetSpriteSheet(const std::string& filename,
        int totalRows,
        int totalColumns,
        int targetRow,
        float frameDuration)
    {
        m_texture = ResourceManager::GetInstance().LoadTexture(filename);
        int texW = 0, texH = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(),
            nullptr, nullptr, &texW, &texH);

        int fw = texW / totalColumns;
        int fh = texH / totalRows;
        m_frames.clear();
        for (int c = 0; c < totalColumns; ++c)
            m_frames.push_back({ c * fw, targetRow * fh, fw, fh });

        m_frameDuration = frameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }

    void SpriteSheetComponent::Update(float deltaTime)
    {
        if (!m_Visible || m_frameDuration <= 0.f || m_frames.empty())
            return;

        m_elapsedTime += deltaTime;
        if (m_Loop) {
            while (m_elapsedTime >= m_frameDuration) {
                m_elapsedTime -= m_frameDuration;
                m_currentFrame = (m_currentFrame + 1) % m_frames.size();
            }
        }
        else {
            while (m_elapsedTime >= m_frameDuration &&
                m_currentFrame < m_frames.size() - 1) {
                m_elapsedTime -= m_frameDuration;
                m_currentFrame++;
            }
        }
    }

    void SpriteSheetComponent::Render() const
    {
        if (!m_texture || m_frames.empty() || !m_Visible)
            return;

        glm::vec3 pos = GetOwner()->GetTransform().GetWorldPosition();
        pos -= glm::vec3{ Camera::GetInstance().GetOffset(), 0.f };

        SDL_Rect src = m_frames[m_currentFrame];
        float w = float(src.w) * m_Scale.x;
        float h = float(src.h) * m_Scale.y;

        pos.x -= w * 0.5f;
        pos.y -= h * 0.5f;

        Renderer::GetInstance().RenderTexture(
            *m_texture, pos.x, pos.y, w, h, &src
        );
    }

    void SpriteSheetComponent::ChangeAnimationRow(int totalRows,
        int totalColumns,
        int targetRow,
        float newFrameDuration)
    {
        if (!m_texture) return;

        int texW = 0, texH = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(),
            nullptr, nullptr, &texW, &texH);
        int fw = texW / totalColumns;
        int fh = texH / totalRows;

        m_frames.clear();
        for (int c = 0; c < totalColumns; ++c)
            m_frames.push_back({ c * fw, targetRow * fh, fw, fh });

        m_frameDuration = newFrameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }

    void SpriteSheetComponent::ChangeAnimation(SpriteSheetComponent::AnimationState newState,
        int totalRows,
        int totalColumns,
        int targetRow,
        int startColumn,
        int frameCount,
        float newFrameDuration,
        bool loop)
    {
        if (m_currentState == newState || !m_texture)
            return;
        m_currentState = newState;

        int texW = 0, texH = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(),
            nullptr, nullptr, &texW, &texH);
        int fw = texW / totalColumns;
        int fh = texH / totalRows;

        m_frames.clear();
        for (int c = startColumn; c < startColumn + frameCount; ++c)
            m_frames.push_back({ c * fw, targetRow * fh, fw, fh });

        m_frameDuration = newFrameDuration;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
        m_Loop = loop;
    }

    void SpriteSheetComponent::SetIdleFrame(SpriteSheetComponent::AnimationState newState,
        int totalRows,
        int totalColumns,
        int targetRow,
        int idleColumn)
    {
        if (m_currentState == newState || !m_texture)
            return;
        m_currentState = newState;

        int texW = 0, texH = 0;
        SDL_QueryTexture(m_texture->GetSDLTexture(),
            nullptr, nullptr, &texW, &texH);
        int fw = texW / totalColumns;
        int fh = texH / totalRows;

        m_frames.clear();
        m_frames.push_back({ idleColumn * fw, targetRow * fh, fw, fh });

        m_frameDuration = 0.f;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
        m_Loop = false;
    }

    void SpriteSheetComponent::SetFrame(int frameIndex)
    {
        if (frameIndex >= 0 && frameIndex < static_cast<int>(m_frames.size()))
            m_currentFrame = static_cast<unsigned int>(frameIndex);
    }

} // namespace dae
