#pragma once
#include <string>

namespace dae {
    class GameOverScreenLoader {
    public:
        void LoadGameOverScreen(const std::string& sceneName = "GameOverScreen");
    };
}