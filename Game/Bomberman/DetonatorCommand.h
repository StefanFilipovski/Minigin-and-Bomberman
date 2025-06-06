// DetonatorCommand.h
#pragma once
#include "Command.h"
#include "PlayerComponent.h"

namespace dae {
    class DetonatorCommand : public Command {
    public:
        explicit DetonatorCommand(PlayerComponent* player)
            : m_Player(player) {
        }

        void Execute() override {
            m_Player->DetonateOldestBomb();
        }

    private:
        PlayerComponent* m_Player;
    };
}

