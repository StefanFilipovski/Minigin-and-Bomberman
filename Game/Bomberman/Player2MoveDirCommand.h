#pragma once
#include "Command.h"
#include "Player2BalloonComponent.h"

namespace dae {
    class Player2MoveDirCommand : public Command {
    public:
        Player2MoveDirCommand(Player2BalloonComponent* player2, Player2BalloonComponent::Direction dir, bool pressed)
            : m_Player2(player2), m_Direction(dir), m_Pressed(pressed)
        {
            assert(m_Player2);
        }

        void Execute() override {
            if (m_Pressed) {
                m_Player2->OnMovementPressed(m_Direction);
            }
            else {
                m_Player2->OnMovementReleased(m_Direction);
            }
        }

    private:
        Player2BalloonComponent* m_Player2;
        Player2BalloonComponent::Direction m_Direction;
        bool m_Pressed;
    };
}