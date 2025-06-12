#pragma once
#include "Command.h"

namespace dae {
    class LevelSkipCommand : public Command {
    public:
        LevelSkipCommand() = default;
        void Execute() override;
    };
}