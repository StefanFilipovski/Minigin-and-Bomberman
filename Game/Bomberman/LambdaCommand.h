#pragma once
#include "Command.h"
#include <functional>

namespace dae {
    class LambdaCommand : public Command {
    public:
        explicit LambdaCommand(std::function<void()> func) : m_function(func) {}
        void Execute() override { m_function(); }
    private:
        std::function<void()> m_function;
    };
}
