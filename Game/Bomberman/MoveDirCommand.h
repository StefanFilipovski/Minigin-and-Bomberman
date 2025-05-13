#pragma once
#include "Command.h"
#include "PlayerComponent.h"

namespace dae {

    /// A single command that tells PlayerComponent to start/stop moving
    class MoveDirCommand final : public Command {
    public:
        MoveDirCommand(PlayerComponent* player,
            PlayerComponent::Direction dir,
            bool pressed)
            : m_Player(player), m_Dir(dir), m_Pressed(pressed)
        {
            assert(m_Player && "MoveDirCommand needs a valid PlayerComponent");
        }

        void Execute() override {
            if (m_Pressed)
                m_Player->OnMovementPressed(m_Dir);
            else
                m_Player->OnMovementReleased(m_Dir);
        }

    private:
        PlayerComponent* m_Player;
        PlayerComponent::Direction     m_Dir;
        bool                           m_Pressed;
    };

} // namespace dae
