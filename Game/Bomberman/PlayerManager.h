#pragma once
#include "Singleton.h"
#include <vector>
#include <vec3.hpp>

namespace dae {
    class GameObject;

    class PlayerManager final : public Singleton<PlayerManager> {
    public:
        void RegisterPlayer(GameObject* player, int playerIndex = 0);
        GameObject* GetPlayer(int playerIndex = 0) const;
        GameObject* GetClosestPlayer(const glm::vec3& position) const;
        void ClearPlayers();

    private:
        friend class Singleton<PlayerManager>;
        PlayerManager() = default;

        std::vector<GameObject*> m_Players;
    };
}