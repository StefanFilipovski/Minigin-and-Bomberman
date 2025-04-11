#pragma once
#include "GameActorCommand.h"

namespace dae
{
    class MoveCommand : public GameActorCommand
    {
    public:
        enum class Direction { Left, Right, Up, Down };

       
        MoveCommand(GameActor* actor, Direction direction, float speed = 1.0f)
            : GameActorCommand(actor), m_Direction(direction), m_Speed(speed) {
        }

        void Execute() override
        {
            switch (m_Direction)
            {
            case Direction::Left:
                GetActor()->Move(-m_Speed, 0.0f);
                break;
            case Direction::Right:
                GetActor()->Move(m_Speed, 0.0f);
                break;
            case Direction::Up:
                GetActor()->Move(0.0f, -m_Speed);
                break;
            case Direction::Down:
                GetActor()->Move(0.0f, m_Speed);
                break;
            }
        }

    private:
        Direction m_Direction;
        float m_Speed;
    };
}
