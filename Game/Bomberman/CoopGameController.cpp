#include "CoopGameController.h"
#include "GameEvents.h"
#include "CoopLevelManager.h"
#include "StartScreenLoader.h"
#include "EnemyManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "CoopExitResponder.h"
#include <iostream>

namespace dae {
    void CoopGameController::Update(float deltaTime)
    {
        if (m_GameEnded) {
            m_EndTimer += deltaTime;
            if (m_EndTimer >= m_DelayTime) {
                // Return to start screen (no high score entry for co-op)
                std::cout << "Returning to start screen..." << std::endl;
                StartScreenLoader loader;
                loader.LoadStartScreen();
            }
        }
        else {
            // Process any pending level transitions using CoopLevelManager
            CoopLevelManager::GetInstance().ProcessPendingTransition();
        }
    }

    void CoopGameController::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::PLAYER_DIED && !m_GameEnded) {
            // In co-op mode, if any player dies, the game ends
            m_GameEnded = true;
            m_EndTimer = 0.0f;

            std::cout << "=== GAME OVER ===" << std::endl;
            std::cout << "Co-op game ended!" << std::endl;
            std::cout << "Returning to start screen in " << m_DelayTime << " seconds..." << std::endl;
        }
        else if (event.id == GameEvents::ENEMY_DIED && !m_GameEnded) {
            // Check if all enemies are dead and spawn exit if needed
            if (EnemyManager::GetInstance().GetEnemyCount() <= 0 && !m_ExitSpawned) {
                SpawnCoopExit();
            }
        }
    }

    void CoopGameController::SpawnCoopExit()
    {
        if (m_ExitSpawned) return;
        m_ExitSpawned = true;

        std::cout << "Co-op: All enemies defeated! Spawning exit..." << std::endl;

        auto* scene = SceneManager::GetInstance().GetActiveScene();
        if (!scene) return;

        // Create exit at grid position (10, 10) 
        auto exitGO = std::make_shared<GameObject>();
        float x = 10 * 16.f; // tile 10
        float y = 10 * 16.f + 64.f; // tile 10 + UI offset

        exitGO->AddComponent<TransformComponent>()
            .SetLocalPosition(x, y, 0.f);
        exitGO->AddComponent<RenderComponent>()
            .SetTexture("Exit.tga");

        auto& cc = exitGO->AddComponent<CollisionComponent>();
        cc.SetSize(16.f, 16.f);
        cc.SetResponder(std::make_unique<CoopExitResponder>(exitGO.get()));

        scene->Add(exitGO);
    }
}