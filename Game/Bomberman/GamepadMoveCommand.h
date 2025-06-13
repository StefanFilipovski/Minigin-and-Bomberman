#pragma once
#include "Command.h"
#include "PlayerComponent.h"

namespace dae {
    class GamepadMoveCommand : public Command {
    public:
        GamepadMoveCommand(PlayerComponent* player, PlayerComponent::Direction direction, int playerIndex, int buttonId);
        void Execute() override;

    private:
        PlayerComponent* m_Player;
        PlayerComponent::Direction m_Direction;
        int m_PlayerIndex;
        int m_ButtonId;

        static bool s_DirectionPressed[2][4]; // [player][direction] - track if each direction is currently pressed
        static bool s_LastDirectionState[2][4]; // [player][direction] - track previous state for edge detection
    };
}