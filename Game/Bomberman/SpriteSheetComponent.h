#pragma once
#include "Component.h"
#include <SDL.h>
#include <vector>
#include <string>
#include <memory>
#include <Renderer.h>
#include "Texture2D.h"
#include <vec2.hpp>

namespace dae {

    class SpriteSheetComponent : public Component {
    public:
        explicit SpriteSheetComponent(GameObject* owner);
        virtual ~SpriteSheetComponent() = default;

        // Load an explicit list of frames.
        void SetSpriteSheet(const std::string& filename,
            const std::vector<SDL_Rect>& frames,
            float frameDuration);

        // Slice a single row out of a grid.
        void SetSpriteSheet(const std::string& filename,
            int totalRows, int totalColumns,
            int targetRow,
            float frameDuration);

        // Advance the current animation by deltaTime.
        void Update(float deltaTime);

        // Draw the current frame (if visible).
        void Render() const override;

        // Scale and loop control.
        void SetScale(float x, float y) { m_Scale = { x, y }; }
        glm::vec2 GetScale() const { return m_Scale; }
        void SetLoop(bool loop) { m_Loop = loop; }
        bool GetLoop() const { return m_Loop; }
        void Reset() { m_currentFrame = 0; m_elapsedTime = 0.f; }

        // Visibility.
        unsigned int GetCurrentFrame() const { return m_currentFrame; }
        unsigned int GetFrameCount()   const { return static_cast<unsigned int>(m_frames.size()); }
        void Hide() { m_Visible = false; }
        void Show() { m_Visible = true; }

        // Change to a full-row animation.
        void ChangeAnimationRow(int totalRows,
            int totalColumns,
            int targetRow,
            float newFrameDuration);

        // Change to a sub-set of frames in one row.
        enum class AnimationState {
            None, Idle,
            MoveUp, MoveRight, MoveDown, MoveLeft,
            Death
        };
        void ChangeAnimation(AnimationState newState,
            int totalRows,
            int totalColumns,
            int targetRow,
            int startColumn,
            int frameCount,
            float newFrameDuration,
            bool loop);

        // Display exactly one frame (for idle).
        void SetIdleFrame(AnimationState newState,
            int totalRows,
            int totalColumns,
            int targetRow,
            int idleColumn);

        // Force a specific index in the current m_frames vector.
        void SetFrame(int frameIndex);

    private:
        std::shared_ptr<Texture2D> m_texture;
        std::vector<SDL_Rect>      m_frames;
        float                      m_frameDuration{ 0.f };
        unsigned int               m_currentFrame{ 0 };
        float                      m_elapsedTime{ 0.f };
        glm::vec2                  m_Scale{ 1.f, 1.f };
        bool                       m_Loop{ true };
        bool                       m_Visible{ true };
        AnimationState             m_currentState{ AnimationState::None };
    };

} // namespace dae
