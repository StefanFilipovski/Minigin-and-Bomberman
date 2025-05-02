#pragma once
#include "Command.h"
#include "PlayerComponent.h"

namespace dae {

    // A Command that moves your PlayerComponent by a fixed amount.
    class PlayerMoveCommand : public Command {
    public:
        enum class Direction { Left, Right, Up, Down };

        PlayerMoveCommand(PlayerComponent* player, Direction dir, float speed)
            : m_Player(player), m_Dir(dir), m_Speed(speed)
        {
            // must have the component
            assert(m_Player && "PlayerMoveCommand needs a valid PlayerComponent");
        }

        void Execute() override {
            switch (m_Dir) {
            case Direction::Left:  m_Player->Move(-m_Speed, 0.f); break;
            case Direction::Right: m_Player->Move(+m_Speed, 0.f); break;
            case Direction::Up:    m_Player->Move(0.f, -m_Speed); break;
            case Direction::Down:  m_Player->Move(0.f, +m_Speed); break;
            }
        }

    private:
        PlayerComponent* m_Player;
        Direction          m_Dir;
        float              m_Speed;
    };

}
