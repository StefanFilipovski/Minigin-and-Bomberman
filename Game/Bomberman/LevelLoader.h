#pragma once
#include <string>

namespace dae {

    
    class LevelLoader {
    public:
        // Loads the level from a file and creates a scene with the specified name.
        void LoadLevel(const std::string& filename, const std::string& sceneName);
    };

} // namespace dae
