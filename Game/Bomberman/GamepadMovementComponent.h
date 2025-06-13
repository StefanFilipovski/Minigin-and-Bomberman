#pragma once
#include "Component.h"
#include "PlayerComponent.h"
#include "GamePad.h"
#include <iostream>

namespace dae {
    class GamepadMovementComponent : public Component {
    public:
        GamepadMovementComponent(GameObject* owner, PlayerComponent* player, int gamepadIndex)
            : Component(owner), m_Player(player), m_GamepadIndex(gamepadIndex), m_Gamepad(gamepadIndex) {
        }

        void Update(float deltaTime) override {
            if (!m_Player) return;

            (void)deltaTime;
            // Poll gamepad state directly
            bool leftPressed = m_Gamepad.IsButtonPressed(GamepadButton::DPadLeft);
            bool rightPressed = m_Gamepad.IsButtonPressed(GamepadButton::DPadRight);
            bool upPressed = m_Gamepad.IsButtonPressed(GamepadButton::DPadUp);
            bool downPressed = m_Gamepad.IsButtonPressed(GamepadButton::DPadDown);

            // Handle Left
            if (leftPressed && !m_LastLeftPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": START LEFT" << std::endl;
                m_Player->OnMovementPressed(PlayerComponent::Direction::Left);
            }
            else if (!leftPressed && m_LastLeftPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": STOP LEFT" << std::endl;
                m_Player->OnMovementReleased(PlayerComponent::Direction::Left);
            }

            // Handle Right
            if (rightPressed && !m_LastRightPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": START RIGHT" << std::endl;
                m_Player->OnMovementPressed(PlayerComponent::Direction::Right);
            }
            else if (!rightPressed && m_LastRightPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": STOP RIGHT" << std::endl;
                m_Player->OnMovementReleased(PlayerComponent::Direction::Right);
            }

            // Handle Up
            if (upPressed && !m_LastUpPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": START UP" << std::endl;
                m_Player->OnMovementPressed(PlayerComponent::Direction::Up);
            }
            else if (!upPressed && m_LastUpPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": STOP UP" << std::endl;
                m_Player->OnMovementReleased(PlayerComponent::Direction::Up);
            }

            // Handle Down
            if (downPressed && !m_LastDownPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": START DOWN" << std::endl;
                m_Player->OnMovementPressed(PlayerComponent::Direction::Down);
            }
            else if (!downPressed && m_LastDownPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": STOP DOWN" << std::endl;
                m_Player->OnMovementReleased(PlayerComponent::Direction::Down);
            }

            // Handle Bomb (A button)
            bool bombPressed = m_Gamepad.IsButtonPressed(GamepadButton::A);
            if (bombPressed && !m_LastBombPressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": BOMB" << std::endl;
                // Need to get scene reference - we'll pass it in constructor
                if (m_Scene) {
                    m_Player->PlaceBomb(*m_Scene);
                }
            }

            // Handle Detonator (B button)
            bool detonatePressed = m_Gamepad.IsButtonPressed(GamepadButton::B);
            if (detonatePressed && !m_LastDetonatePressed) {
                std::cout << "Gamepad " << m_GamepadIndex << ": DETONATE" << std::endl;
                m_Player->DetonateOldestBomb();
            }

            // Update previous states
            m_LastLeftPressed = leftPressed;
            m_LastRightPressed = rightPressed;
            m_LastUpPressed = upPressed;
            m_LastDownPressed = downPressed;
            m_LastBombPressed = bombPressed;
            m_LastDetonatePressed = detonatePressed;
        }

        void SetScene(Scene* scene) { m_Scene = scene; }

    private:
        PlayerComponent* m_Player;
        int m_GamepadIndex;
        Gamepad m_Gamepad;
        Scene* m_Scene{ nullptr };

        // Previous button states for edge detection
        bool m_LastLeftPressed{ false };
        bool m_LastRightPressed{ false };
        bool m_LastUpPressed{ false };
        bool m_LastDownPressed{ false };
        bool m_LastBombPressed{ false };
        bool m_LastDetonatePressed{ false };
    };
}