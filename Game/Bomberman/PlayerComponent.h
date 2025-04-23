#pragma once
#include "Component.h"
#include <SDL.h>
#include <vector>
#include <vec3.hpp>

// forward declarations
namespace dae {
    class SpriteSheetComponent;
    class TransformComponent;

    class PlayerComponent : public Component {
    public:
        explicit PlayerComponent(GameObject* owner);
        ~PlayerComponent() override = default;

        void Update(float deltaTime) override;

        // collision helpers
        void BeginMove();
        void Move(float dx, float dy);
        void RevertMove();

        // health
        int  GetHealth() const;
        void TakeDamage(int dmg);

        // input-driven sprite & movement
        void OnInputPressed(SDL_Scancode key);
        void OnInputReleased(SDL_Scancode key);

    private:
        void MoveCurrent(float deltaTime);
        void UpdateSpriteState();

        // cached component pointers (do NOT add these in here!)
        SpriteSheetComponent* m_Sprite{ nullptr };
        TransformComponent* m_Transform{ nullptr };

        // collision
        glm::vec3 m_lastValidPosition{ 0.f, 0.f, 0.f };

        // health and death flag
        int   m_health{ 3 };
        bool  m_IsDead{ false };

        // speed
        float m_speed{ 100.f };

        // movement stack
        std::vector<SDL_Scancode> m_MovementKeys;
    };
}
