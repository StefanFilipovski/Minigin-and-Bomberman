#pragma once
#include <string>

namespace dae {
    class StartScreenLoader {
    public:
        void LoadStartScreen(const std::string& sceneName = "StartScreen");
    };
}