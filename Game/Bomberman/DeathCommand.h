#pragma once
#include "Command.h"
#include "GameActor.h"

namespace dae {

   
    class DeathCommand : public Command
    {
    public:
        explicit DeathCommand(GameActor* actor)
            : m_Actor(actor) {
        }

        void Execute() override
        {
            if (m_Actor)
            {
               
                m_Actor->TakeDamage(1);
            }
        }

    private:
        GameActor* m_Actor;
    };

}
