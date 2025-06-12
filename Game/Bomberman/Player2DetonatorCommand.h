#pragma once
#include "Command.h"
#include "Player2BalloonComponent.h"

namespace dae {
    class Player2DetonatorCommand : public Command {
    public:
        Player2DetonatorCommand(Player2BalloonComponent* player2)
            : m_Player2(player2)
        {
            assert(m_Player2);
        }

        void Execute() override {
            m_Player2->DetonateOldestBomb();
        }

    private:
        Player2BalloonComponent* m_Player2;
    };
}