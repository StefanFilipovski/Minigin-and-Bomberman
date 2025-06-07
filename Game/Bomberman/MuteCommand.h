#pragma once
#include "Command.h"
#include "ServiceLocator.h"
#include <iostream>

namespace dae {
    class MuteCommand : public Command {
    public:
        void Execute() override {
            ServiceLocator::GetSoundSystem().ToggleMute();

            // Optional: Print mute status
            if (ServiceLocator::GetSoundSystem().IsMuted()) {
                std::cout << "Sound muted\n";
            }
            else {
                std::cout << "Sound unmuted\n";
            }
        }
    };
}