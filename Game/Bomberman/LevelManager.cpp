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
#include "ServiceLocator.h"
#include "LivesManager.h"
#include "ScoreManager.h"
#include "GameState.h"
#include <thread>
#include <chrono>
#include "CollisionComponent.h"

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

        // If we're already transitioning, ignore this call
        if (m_IsTransitioning) {
            std::cerr << "Already transitioning, ignoring LoadLevel call" << std::endl;
            return;
        }

        m_IsTransitioning = true;

        // Set cleanup state first
        GameStateManager::SetState(GameState::Cleanup);

        // Stop all sounds
        ServiceLocator::GetSoundSystem().StopMusic();
        ServiceLocator::GetSoundSystem().StopAllSounds();

        // Get current scene name
        std::string oldSceneName = "Level " + std::to_string(m_CurrentLevel + 1);

        // Phase 1: Stop all updates by deactivating the scene
        if (auto* oldScene = SceneManager::GetInstance().GetScene(oldSceneName)) {
            oldScene->SetActive(false);
        }

        // Wait to ensure no updates are in progress
        std::this_thread::sleep_for(std::chrono::milliseconds(32));

        // Phase 2: Clear all manager references IN ORDER
        // UI first (they reference game objects)
        ScoreManager::GetInstance().SetScoreDisplay(nullptr);
        LivesManager::GetInstance().SetLivesDisplay(nullptr);

        // Game objects second
        PlayerManager::GetInstance().ClearPlayers();
        EnemyManager::GetInstance().ClearLevel();

        // Systems last
        InputManager::GetInstance().ClearAllBindings();

        // Special handling for collision manager - clear in stages
        auto& collisionMgr = CollisionManager::GetInstance();
        // First, clear all responders to prevent callbacks
        for (auto* comp : collisionMgr.GetComponents()) {
            if (comp) {
                comp->SetResponder(nullptr);
            }
        }
        // Then clear the collision manager
        collisionMgr.Clear();

        // Wait again
        std::this_thread::sleep_for(std::chrono::milliseconds(32));

        // Phase 3: Remove the old scene
        if (SceneManager::GetInstance().GetScene(oldSceneName) != nullptr) {
            // Set active scene to null first
            if (SceneManager::GetInstance().GetActiveScene() == SceneManager::GetInstance().GetScene(oldSceneName)) {
                SceneManager::GetInstance().SetActiveScene(nullptr);
            }
            SceneManager::GetInstance().RemoveScene(oldSceneName);
        }

        // Wait for destruction to complete
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // Now safe to transition
        GameStateManager::SetState(GameState::LevelTransition);

        // Update level index
        m_CurrentLevel = levelIndex;
        std::string sceneName = "Level " + std::to_string(m_CurrentLevel + 1);

        // Create new scene
        LevelLoader loader;
        loader.LoadLevel(m_LevelFiles[m_CurrentLevel], sceneName);

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);

        // Handle score/lives based on transition type
        if (levelIndex > 0) {
            // Next level - keep score, reset lives
            LivesManager::GetInstance().ResetLives();
        }
        else {
            // First level or restart - reset everything
            ScoreManager::GetInstance().ResetScore();
            LivesManager::GetInstance().ResetLives();
        }

        // Small delay before resuming
        std::this_thread::sleep_for(std::chrono::milliseconds(32));

        // Resume normal game state
        GameStateManager::SetState(GameState::Playing);
        m_IsTransitioning = false;

        // Start music
        ServiceLocator::GetSoundSystem().PlayMusic("Bomberman (NES) Music - Stage Theme.ogg", 0.7f);
    }

    void LevelManager::LoadNextLevel()
    {
        if (IsLastLevel()) {
            // Game complete!
            std::cout << "Game Complete! All levels finished!" << std::endl;
            m_TransitionQueued = false;
            GameOverManager::GetInstance().TriggerGameOver();
        }
        else {
            m_TransitionQueued = false;
            LoadLevel(m_CurrentLevel + 1);
        }
    }

    void LevelManager::ResetToFirstLevel()
    {
        m_TransitionQueued = false;
        ScoreManager::GetInstance().ResetScore();
        LoadLevel(0);
    }

    void LevelManager::ProcessPendingTransition()
    {
        if (m_TransitionQueued && !m_IsTransitioning) {
            LoadNextLevel();
        }
    }

    void LevelManager::QueueLevelTransition()
    {
        if (!m_TransitionQueued && !m_IsTransitioning) {
            m_TransitionQueued = true;
        }
    }
}