#include "EnemyManager.h"
#include "BaseEnemyComponent.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ExitResponder.h"
#include "GameEvents.h"
#include <random>

namespace dae {
    void EnemyManager::RegisterEnemy(BaseEnemyComponent* enemy)
    {
        if (enemy) {
            enemy->AddObserver(this);
            m_EnemyCount++;
        }
    }

    void EnemyManager::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::ENEMY_DIED) {
            m_EnemyCount--;
            if (m_EnemyCount <= 0 && m_CurrentScene && !m_ExitSpawned) {
                // Delay spawn slightly to ensure we're not mid-collision-check
                SDL_AddTimer(100, // 100ms delay
                    [](Uint32, void* param) -> Uint32 {
                        auto* mgr = static_cast<EnemyManager*>(param);
                        if (mgr && mgr->m_CurrentScene) {
                            mgr->SpawnExitIfNeeded(mgr->m_CurrentScene);
                        }
                        return 0;
                    },
                    this
                );
            }
        }
    }

    void EnemyManager::SpawnExitIfNeeded(Scene* scene)
    {
        // Use the stored scene if no scene is passed
        if (!scene) {
            scene = m_CurrentScene;
        }

        if (m_ExitSpawned || !scene) return;
        m_ExitSpawned = true;

        // Create exit at a random walkable position
        auto exitGO = std::make_shared<GameObject>();

        // Place at grid position (10, 10) for example
        float x = 10 * 16.f; // tile 10
        float y = 10 * 16.f + 64.f; // tile 10 + UI offset

        exitGO->AddComponent<TransformComponent>()
            .SetLocalPosition(x, y, 0.f);
        exitGO->AddComponent<RenderComponent>()
            .SetTexture("Exit.tga");

        auto& cc = exitGO->AddComponent<CollisionComponent>();
        cc.SetSize(16.f, 16.f);
        cc.SetResponder(std::make_unique<ExitResponder>(exitGO.get()));

        scene->Add(exitGO);
    }

    void EnemyManager::ClearLevel()
    {
        m_EnemyCount = 0;
        m_ExitSpawned = false;
        m_CurrentScene = nullptr;
    }

    Scene* EnemyManager::GetCurrentScene() const
    {
        return m_CurrentScene;
    }
}