#pragma once
#include "Singleton.h"
#include "Observer.h"
#include <vector>

namespace dae {
    class BaseEnemyComponent;
    class Scene;

    class EnemyManager : public Singleton<EnemyManager>, public Observer {
    public:
        void RegisterEnemy(BaseEnemyComponent* enemy);
        void OnNotify(const Event& event) override;
        void SpawnExitIfNeeded(Scene* scene);
        void ClearLevel();
        Scene* GetCurrentScene() const;
        void SetCurrentScene(Scene* scene) { m_CurrentScene = scene; } 

        int GetEnemyCount() const { return m_EnemyCount; }

    private:
        friend class Singleton<EnemyManager>;
        EnemyManager() = default;

        int m_EnemyCount{ 0 };
        Scene* m_CurrentScene{ nullptr };
        bool m_ExitSpawned{ false };
        std::vector<Event> m_PendingNotifications;
    };
}