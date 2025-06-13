#pragma once
#include "Singleton.h"
#include <string>
#include <vector>

namespace dae {
    class LevelManager : public Singleton<LevelManager> {
    public:
        void Initialize();
        void LoadLevel(int levelIndex);
        void LoadNextLevel();
        void ResetToFirstLevel();

      
        void QueueLevelTransition() { m_TransitionQueued = true; }
        void ProcessPendingTransition();

        int GetCurrentLevel() const { return m_CurrentLevel; }
        bool IsLastLevel() const { return m_CurrentLevel >= (int)m_LevelFiles.size() - 1; }

    private:
        friend class Singleton<LevelManager>;
        LevelManager() = default;

        std::vector<std::string> m_LevelFiles;
        int m_CurrentLevel{ 0 };
        bool m_TransitionQueued{ false };  
    };
}