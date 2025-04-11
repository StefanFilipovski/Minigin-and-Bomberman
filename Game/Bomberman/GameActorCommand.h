#pragma once
#include "Command.h"
#include "GameActor.h"
#include <iostream>

namespace dae
{
    class GameActorCommand : public Command
    {
    public:
        explicit GameActorCommand(GameActor* actor)
            : m_Actor(actor)
        {
            std::cout << "[GameActorCommand] Created for actor at memory: " << actor << std::endl;
        }

        virtual ~GameActorCommand() = default;

    protected:
        GameActor* GetActor() const
        {
            return m_Actor;
        }

    private:
        GameActor* m_Actor;
    };
}
