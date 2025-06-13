#pragma once
#include <string>

namespace dae {
    class CoopGameLoader {
    public:
        void LoadCoopGame(const std::string& filename, const std::string& sceneName = "CoopGame");
    };
}