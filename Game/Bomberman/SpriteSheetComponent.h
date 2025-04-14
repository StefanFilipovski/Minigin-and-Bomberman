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

        
        void SetSpriteSheet(const std::string& filename, const std::vector<SDL_Rect>& frames, float frameDuration);
        void SetSpriteSheet(const std::string& filename, int totalRows, int totalColumns, int targetRow, float frameDuration);
        void Update(float deltaTime);
        void Render() const override;
        void SetScale(float x, float y) { m_Scale = { x, y }; }
        glm::vec2 GetScale() const { return m_Scale; }
        void SetLoop(bool loop) { m_Loop = loop; }
        bool GetLoop() const { return m_Loop; }
        void Reset() { m_currentFrame = 0; m_elapsedTime = 0.f; }

        // Full-row animation change.
        void ChangeAnimationRow(int totalRows, int totalColumns, int targetRow, float newFrameDuration);

        // Manually set the current frame.
        void SetFrame(int frameIndex);

        // **********************************************
        // New interface for partial animations and idle.
        // **********************************************

        // Animation state enumeration.
        enum class AnimationState { None, Idle, MoveUp, MoveRight, MoveDown, MoveLeft, Death };

        // Change the animation only if the state is different.
        // Loads a subset of frames (starting at startColumn for frameCount frames).
        void ChangeAnimation(AnimationState newState, int totalRows, int totalColumns, int targetRow, int startColumn, int frameCount, float newFrameDuration, bool loop);

        // Set the sprite to a single idle frame.
        void SetIdleFrame(AnimationState newState, int totalRows, int totalColumns, int targetRow, int idleColumn);

        // **********************************************
        // New methods for movement key handling.
        // These methods encapsulate the "movement stack" logic.
        // **********************************************
        // To be called when a movement key is pressed.
        void OnMovementKeyPressed(SDL_Scancode key);

        // To be called when a movement key is released.
        void OnMovementKeyReleased(SDL_Scancode key);

    private:
        // Helper that updates the animation based on the movement keys currently pressed.
        void UpdateMovementAnimation();

        std::shared_ptr<Texture2D> m_texture;   // Loaded texture.
        std::vector<SDL_Rect> m_frames;           // Source rectangles for frames.
        float m_frameDuration{ 0.f };             // Time per frame.
        unsigned int m_currentFrame{ 0 };         // Current frame index.
        float m_elapsedTime{ 0.f };               // Accumulated time.
        glm::vec2 m_Scale{ 1.0f, 1.0f };          // Scaling factor.
        bool m_Loop{ true };                      // Should the animation loop.

        // Tracks the current animation state.
        AnimationState m_currentState{ AnimationState::None };

        // Movement key stack: order in which movement keys were pressed.
        std::vector<SDL_Scancode> m_MovementKeys;
    };

} // namespace dae
