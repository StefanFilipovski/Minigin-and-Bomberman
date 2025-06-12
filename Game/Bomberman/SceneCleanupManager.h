#pragma once
#include <functional>
#include <vector>

namespace dae {
    // Helper class to ensure proper cleanup order
    class SceneCleanupManager {
    public:
        static SceneCleanupManager& GetInstance() {
            static SceneCleanupManager instance;
            return instance;
        }

        void RegisterCleanupCallback(std::function<void()> callback) {
            m_CleanupCallbacks.push_back(callback);
        }

        void ExecuteCleanup() {
            // Execute all cleanup callbacks in reverse order (LIFO)
            for (auto it = m_CleanupCallbacks.rbegin(); it != m_CleanupCallbacks.rend(); ++it) {
                (*it)();
            }
            m_CleanupCallbacks.clear();
        }

    private:
        SceneCleanupManager() = default;
        std::vector<std::function<void()>> m_CleanupCallbacks;
    };
}