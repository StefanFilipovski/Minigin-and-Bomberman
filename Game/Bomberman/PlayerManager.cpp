#include "PlayerManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace dae {

    void PlayerManager::RegisterPlayer(GameObject* player, int playerIndex)
    {
        if (playerIndex >= static_cast<int>(m_Players.size())) {
            m_Players.resize(playerIndex + 1, nullptr);
        }
        m_Players[playerIndex] = player;
    }

    GameObject* PlayerManager::GetPlayer(int playerIndex) const
    {
        if (playerIndex < 0 || playerIndex >= static_cast<int>(m_Players.size())) {
            return nullptr;
        }
        return m_Players[playerIndex];
    }

    GameObject* PlayerManager::GetClosestPlayer(const glm::vec3& position) const
    {
        GameObject* closest = nullptr;
        float minDistance = std::numeric_limits<float>::max();

        for (auto* player : m_Players) {
            if (!player) continue;

            auto& transform = player->GetTransform();
            auto playerPos = transform.GetLocalPosition();

            // Calculate distance manually with proper float casting
            float dx = position.x - playerPos.x;
            float dy = position.y - playerPos.y;
            float dz = position.z - playerPos.z;
            float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

            if (dist < minDistance) {
                minDistance = dist;
                closest = player;
            }
        }

        return closest;
    }

    void PlayerManager::ClearPlayers()
    {
        m_Players.clear();
    }
}