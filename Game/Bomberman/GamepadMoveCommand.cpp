#include "GamepadMoveCommand.h"
#include "GamePad.h"
#include "InputManager.h"
#include <iostream>

namespace dae {
    // Static member definitions
    bool GamepadMoveCommand::s_DirectionPressed[2][4] = { {false} };
    bool GamepadMoveCommand::s_LastDirectionState[2][4] = { {false} };

    GamepadMoveCommand::GamepadMoveCommand(PlayerComponent* player, PlayerComponent::Direction direction, int playerIndex, int buttonId)
        : m_Player(player), m_Direction(direction), m_PlayerIndex(playerIndex), m_ButtonId(buttonId)
    {
    }

    void GamepadMoveCommand::Execute()
    {
        //if (!m_Player) return;

        //// Create a gamepad object to check current state
        //Gamepad gamepad(m_PlayerIndex);
        //bool isPressed = gamepad.IsButtonPressed(static_cast<GamepadButton>(m_ButtonId));

        //int dirIndex = static_cast<int>(m_Direction);

        //// Update current state
        //bool wasPressed = s_LastDirectionState[m_PlayerIndex][dirIndex];
        //s_DirectionPressed[m_PlayerIndex][dirIndex] = isPressed;
        //s_LastDirectionState[m_PlayerIndex][dirIndex] = isPressed;

        //// Handle button press (start movement)
        //if (isPressed && !wasPressed) {
        //    std::cout << "Gamepad " << m_PlayerIndex << " started moving " << dirIndex << std::endl;
        //    m_Player->SetMovementDirection(m_Direction, true);
        //}
        //// Handle button release (stop movement)
        //else if (!isPressed && wasPressed) {
        //    std::cout << "Gamepad " << m_PlayerIndex << " stopped moving " << dirIndex << std::endl;
        //    m_Player->SetMovementDirection(m_Direction, false);

        //    // Check if any other direction is still pressed and prioritize the most recent
        //    bool anyDirectionPressed = false;
        //    for (int i = 0; i < 4; ++i) {
        //        if (s_DirectionPressed[m_PlayerIndex][i]) {
        //            anyDirectionPressed = true;
        //            break;
        //        }
        //    }

        //    // If no direction is pressed, stop all movement
        //    if (!anyDirectionPressed) {
        //        m_Player->StopMoving();
        //    }
        //}
    }
}