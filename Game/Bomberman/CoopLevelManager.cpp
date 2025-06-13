#include "CoopLevelManager.h"
#include "CoopGameLoader.h"
#include "SceneManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "CollisionManager.h"
#include "InputManager.h"
#include "Scene.h"
#include <iostream>
#include "StartScreenLoader.h"
#include "ScoreManager.h"
#include <ServiceLocator.h>

namespace dae {
    void CoopLevelManager::Initialize()
    {
        // Define all level files (same as story mode)
        m_LevelFiles = {
            "../Data/level1.txt",
            "../Data/level2.txt",
            "../Data/level3.txt"
        };

        m_CurrentLevel = 0;
    }

    void CoopLevelManager::LoadLevel(int levelIndex)
    {
        std::cout << "=== LOADING CO-OP LEVEL " << levelIndex << " ===" << std::endl;

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
            std::string sceneName = "CoopLevel " + std::to_string(m_CurrentLevel + 1);
            std::cout << "Creating scene: " << sceneName << std::endl;

            // Load the co-op level
            std::cout << "Loading co-op level file: " << m_LevelFiles[m_CurrentLevel] << std::endl;
            CoopGameLoader loader;
            loader.LoadCoopGame(m_LevelFiles[m_CurrentLevel], sceneName);

            std::cout << "Setting active scene to: " << sceneName << std::endl;
            sceneManager.SetActiveScene(sceneName);

            std::cout << "=== CO-OP LEVEL " << levelIndex << " LOADED SUCCESSFULLY ===" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "EXCEPTION in CoopLevelManager::LoadLevel: " << e.what() << std::endl;
            throw;
        }
    }

    void CoopLevelManager::LoadNextLevel()
    {
        std::cout << "=== TRANSITION TO NEXT CO-OP LEVEL ===" << std::endl;
        std::cout << "Current level: " << m_CurrentLevel << std::endl;
        std::cout << "Is last level: " << (IsLastLevel() ? "yes" : "no") << std::endl;

        if (IsLastLevel()) {
            std::cout << "Co-op Game Complete! All levels finished!" << std::endl;
            std::cout << "Final Score: " << ScoreManager::GetInstance().GetScore() << std::endl;

            // For co-op mode, just return to start screen (no high score entry)
            std::cout << "Returning to start screen..." << std::endl;
            StartScreenLoader loader;
            loader.LoadStartScreen();
        }
        else {
            std::cout << "Loading next co-op level: " << (m_CurrentLevel + 1) << std::endl;
            LoadLevel(m_CurrentLevel + 1);
        }
    }

    void CoopLevelManager::ResetToFirstLevel()
    {
        // Only clear input bindings - let scene creation handle the rest
        InputManager::GetInstance().ClearAllBindings();

        // Reset score
        ScoreManager::GetInstance().ResetScore();

        // Simply load level 0
        LoadLevel(0);
    }

    void CoopLevelManager::ProcessPendingTransition()
    {
        if (m_TransitionQueued) {
            m_TransitionQueued = false;
            LoadNextLevel();
        }
    }
}