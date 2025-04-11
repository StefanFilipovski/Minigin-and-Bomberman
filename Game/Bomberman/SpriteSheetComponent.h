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

        // Existing method to set spritesheet via frames vector.
        void SetSpriteSheet(const std::string& filename, const std::vector<SDL_Rect>& frames, float frameDuration);

        // New overload calculates frames based on rows and columns.
        void SetSpriteSheet(const std::string& filename, int totalRows, int totalColumns, int targetRow, float frameDuration);

        // Update the animation based on delta time.
        void Update(float deltaTime);

        // Render the current frame.
        void Render() const override;

        // Set scale.
        void SetScale(float x, float y) { m_Scale = { x, y }; }
        glm::vec2 GetScale() const { return m_Scale; }

        // Set whether the animation loops.
        void SetLoop(bool loop) { m_Loop = loop; }
        bool GetLoop() const { return m_Loop; }

        // Optional: Reset the animation.
        void Reset() { m_currentFrame = 0; m_elapsedTime = 0.f; }

        // Method to change the active animation row dynamically.
        void ChangeAnimationRow(int totalRows, int totalColumns, int targetRow, float newFrameDuration);

    private:
        std::shared_ptr<Texture2D> m_texture;   // Loaded texture from ResourceManager.
        std::vector<SDL_Rect> m_frames;           // Source rectangles for animation frames.
        float m_frameDuration{ 0.f };            // Time (in seconds) per frame.
        unsigned int m_currentFrame{ 0 };        // Current animation frame.
        float m_elapsedTime{ 0.f };              // Accumulated time since last frame change.
        glm::vec2 m_Scale{ 1.0f, 1.0f };         // Scale for rendering.
        bool m_Loop{ true };                     // Whether to loop the animation.
    };

} // namespace dae
