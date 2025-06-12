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
#include "ScoreManager.h"
#include <ServiceLocator.h>

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
        std::cout << "=== LOADING LEVEL " << levelIndex << " ===" << std::endl;

        try {
            if (levelIndex < 0 || levelIndex >= (int)m_LevelFiles.size()) {
                std::cerr << "Invalid level index: " << levelIndex << std::endl;
                return;
            }

            // Debug: Check current scene before loading
            auto& sceneManager = SceneManager::GetInstance();
            auto* currentScene = sceneManager.GetActiveScene();
            std::cout << "Current active scene before load: " << (currentScene ? "exists" : "null") << std::endl;

            // SAFER CLEANUP ORDER: Clear observers first, then managers
            if (currentScene != nullptr) {
                std::cout << "Active scene detected - doing safer cleanup..." << std::endl;

                // 1. First clear input bindings (stops new events)
                std::cout << "Clearing input bindings first..." << std::endl;
                InputManager::GetInstance().ClearAllBindings();

                // 2. Stop all sounds
                std::cout << "Stopping music..." << std::endl;
                ServiceLocator::GetSoundSystem().StopMusic();

                // 3. Clear managers in safer order (enemies first, then players, then collision)
                std::cout << "Clearing EnemyManager..." << std::endl;
                EnemyManager::GetInstance().ClearLevel();

                std::cout << "Clearing PlayerManager..." << std::endl;
                PlayerManager::GetInstance().ClearPlayers();

                std::cout << "Clearing CollisionManager..." << std::endl;
                CollisionManager::GetInstance().Clear();

                // 4. Give the scene a chance to clean up before replacing it
                std::cout << "Letting scene cleanup..." << std::endl;
                // Small delay to let any pending updates finish
            }
            else {
                // Still clear input bindings even for first load
                std::cout << "Clearing input bindings..." << std::endl;
                InputManager::GetInstance().ClearAllBindings();
            }

            // Set current level
            std::cout << "Setting current level to " << levelIndex << std::endl;
            m_CurrentLevel = levelIndex;

            // Create scene name
            std::string sceneName = "Level " + std::to_string(m_CurrentLevel + 1);
            std::cout << "Creating scene: " << sceneName << std::endl;

            // Load the level
            std::cout << "Loading level file: " << m_LevelFiles[m_CurrentLevel] << std::endl;
            LevelLoader loader;
            loader.LoadLevel(m_LevelFiles[m_CurrentLevel], sceneName);

            std::cout << "Setting active scene to: " << sceneName << std::endl;
            sceneManager.SetActiveScene(sceneName);

            std::cout << "=== LEVEL " << levelIndex << " LOADED SUCCESSFULLY ===" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "EXCEPTION in LoadLevel: " << e.what() << std::endl;
            throw;
        }
    }

    void LevelManager::LoadNextLevel()
    {
        std::cout << "=== TRANSITION TO NEXT LEVEL ===" << std::endl;
        std::cout << "Current level: " << m_CurrentLevel << std::endl;
        std::cout << "Is last level: " << (IsLastLevel() ? "yes" : "no") << std::endl;

        if (IsLastLevel()) {
            std::cout << "Game Complete! All levels finished!" << std::endl;
            std::cout << "Final Score: " << ScoreManager::GetInstance().GetScore() << std::endl;
            GameOverManager::GetInstance().TriggerGameOver();
        }
        else {
            std::cout << "Loading next level: " << (m_CurrentLevel + 1) << std::endl;
            LoadLevel(m_CurrentLevel + 1);
        }
    }


    void LevelManager::ResetToFirstLevel()
    {
        // Only clear input bindings - let scene creation handle the rest
        InputManager::GetInstance().ClearAllBindings();

        // Reset score
        ScoreManager::GetInstance().ResetScore();

        // Simply load level 0
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