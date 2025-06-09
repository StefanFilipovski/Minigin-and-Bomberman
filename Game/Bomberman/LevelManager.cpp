#include "LevelManager.h"
#include "LevelLoader.h"
#include "SceneManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "CollisionManager.h"
#include "InputManager.h"
#include "Scene.h"
#include <iostream>
#include "GameOverManager.h"

namespace dae {
    void LevelManager::Initialize()
    {
        // Define all level files
        m_LevelFiles = {
            "../Data/level1.txt",
            "../Data/level2.txt",
            "../Data/level3.txt"
        };

        m_CurrentLevel = 0;
    }

    void LevelManager::LoadLevel(int levelIndex)
    {
        if (levelIndex < 0 || levelIndex >= (int)m_LevelFiles.size()) {
            std::cerr << "Invalid level index: " << levelIndex << std::endl;
            return;
        }

        // Clear all managers first
        EnemyManager::GetInstance().ClearLevel();
        PlayerManager::GetInstance().ClearPlayers();

        // Clear collision manager
        CollisionManager::GetInstance().Clear();

        // Clear input bindings for all players
        InputManager::GetInstance().ClearAllBindings();

        m_CurrentLevel = levelIndex;

        // Create a unique scene name for each level
        std::string sceneName = "Level " + std::to_string(m_CurrentLevel + 1);

        // Load the new level (creates or replaces the scene)
        LevelLoader loader;
        loader.LoadLevel(m_LevelFiles[m_CurrentLevel], sceneName);

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);


    }

    void LevelManager::LoadNextLevel()
    {
        if (IsLastLevel()) {
            // Game complete - for now just restart
             // Game complete!
            std::cout << "Game Complete! All levels finished!" << std::endl;
            GameOverManager::GetInstance().TriggerGameOver();
        }
        else {
            LoadLevel(m_CurrentLevel + 1);
        }
    }

    void LevelManager::ResetToFirstLevel()
    {
        LoadLevel(0);
    }

    void LevelManager::ProcessPendingTransition()
    {
        if (m_TransitionQueued) {
            m_TransitionQueued = false;
            LoadNextLevel();
        }
    }
}