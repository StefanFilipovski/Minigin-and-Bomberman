#pragma once
#include "Command.h"
#include "PlayerMovementComponent.h"
#include <iostream> // <— add this

namespace dae {
    class MoveCommand : public Command {
    public:
        MoveCommand(PlayerMovementComponent* movement, Direction dir)
            : m_movement(movement), m_direction(dir) {
        }

        void Execute() override {
            if (!m_movement) return;
            // TEMP DEBUG
            std::cout << "[MoveCommand] dir=" << static_cast<int>(m_direction) << "\n";
            // Real behavior
            if (m_direction == Direction::None) m_movement->StopMove();
            else m_movement->StartMove(m_direction);
        }

    private:
        PlayerMovementComponent* m_movement{};
        Direction m_direction{ Direction::None };
    };
}
