#pragma once
#include "GameActorCommand.h"

namespace dae
{
    class MoveLeftCommand : public GameActorCommand
    {
    public:
        using GameActorCommand::GameActorCommand;
        void Execute() override { GetActor()->Move(-1.0f, 0.0f); }
    };

    class MoveRightCommand : public GameActorCommand
    {
    public:
        using GameActorCommand::GameActorCommand;
        void Execute() override { GetActor()->Move(1.0f, 0.0f); }
    };

    class MoveUpCommand : public GameActorCommand
    {
    public:
        using GameActorCommand::GameActorCommand;
        void Execute() override { GetActor()->Move(0.0f, -1.0f); }
    };

    class MoveDownCommand : public GameActorCommand
    {
    public:
        using GameActorCommand::GameActorCommand;
        void Execute() override { GetActor()->Move(0.0f, 1.0f); }
    };
}
