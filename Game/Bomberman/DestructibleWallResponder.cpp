#include "DestructibleWallResponder.h"
#include "GameObject.h"
#include "SpriteSheetComponent.h"
#include "CollisionComponent.h"
#include "PlayerComponent.h"
#include "BlastResponder.h"
#include "BaseEnemyComponent.h"
#include <RenderComponent.h>
#include "PowerUpComponent.h"
#include "PowerUpCollisionResponder.h"
#include "PowerUpType.h"
#include "Scene.h"

namespace dae {

    DestructibleWallResponder::DestructibleWallResponder(GameObject* owner)
        : m_pOwner(owner)
    {
        m_pSheet = owner->GetComponent<SpriteSheetComponent>();
        m_pCollider = owner->GetComponent<CollisionComponent>();
    }

    void DestructibleWallResponder::OnCollide(GameObject* other)
    {
        if (m_Started)
            return;

        // Only trigger on blasts
        bool isBlast = false;
        if (!other) {
            isBlast = true;  // manual calls
        }
        else if (auto* otherCC = other->GetComponent<CollisionComponent>()) {
            isBlast = dynamic_cast<BlastResponder*>(otherCC->GetResponder()) != nullptr;
        }

        if (isBlast)
        {
            m_Started = true;

            // Store the position before anything happens
            auto wallPos = m_pOwner->GetTransform().GetLocalPosition();

            // 1) Play the 1×7 crumble animation (0.1 s per frame)
            m_pSheet->ChangeAnimationRow(
                /*totalRows=*/1,
                /*totalColumns=*/7,
                /*targetRow=*/0,
                /*newFrameDuration=*/0.1f
            );
            m_pSheet->SetLoop(false);

            // 2) After 0.5 s, hide the sprite so it disappears
            constexpr Uint32 hideDelayMs = static_cast<Uint32>(5 * 0.1f * 1000);
            SDL_AddTimer(hideDelayMs,
                [](Uint32 /*interval*/, void* param)->Uint32 {
                    static_cast<SpriteSheetComponent*>(param)->Hide();
                    return 0;
                },
                m_pSheet);

            //// Schedule power-up spawn after wall disappears
            //constexpr Uint32 spawnDelayMs = static_cast<Uint32>(6 * 0.1f * 1000);
            //SDL_AddTimer(spawnDelayMs,
            //    [](Uint32 /*interval*/, void* param)->Uint32 {
            //        static_cast<DestructibleWallResponder*>(param)->SpawnPowerUp();
            //        return 0;
            //    },
            //    this);

            // 3) Disable collision so both players and further blasts pass through
            m_pCollider->SetResponder(nullptr);
            SpawnPowerUpAt(wallPos);
        }
        else
        {
            // normal blocking behavior
            if (auto* pc = other->GetComponent<PlayerComponent>()) {
                pc->RevertMove();
                return;
            }

            // Check for ANY enemy type
            if (auto* enemy = other->GetComponent<BaseEnemyComponent>()) {
                enemy->RevertMove();
                return;
            }
        }
    }

    void DestructibleWallResponder::SpawnPowerUpAt(const glm::vec3& position)
    {
        // 30% chance to spawn a power-up
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dropChance(0.0, 1.0);
        static std::uniform_int_distribution<> powerUpType(0, 2);

        if (dropChance(gen) > 0.3f) {
            return; // No power-up
        }

        // Determine which power-up to spawn
        PowerUpType type = static_cast<PowerUpType>(powerUpType(gen));

        // Create power-up at wall's position
        auto powerUpGO = std::make_shared<GameObject>();
        powerUpGO->AddComponent<TransformComponent>()
            .SetLocalPosition(position.x, position.y, position.z);

        // Add render component with appropriate texture
        auto& rc = powerUpGO->AddComponent<RenderComponent>();
        switch (type) {
        case PowerUpType::ExtraBomb:
            rc.SetTexture("PowerUpBomb.tga");
            break;
        case PowerUpType::Detonator:
            rc.SetTexture("PowerUpDetonator.tga");
            break;
        case PowerUpType::FlameRange:
            rc.SetTexture("PowerUpFlame.tga");
            break;
        }

        // Add power-up component
        auto& puc = powerUpGO->AddComponent<PowerUpComponent>(type);

        // Add collision
        auto& cc = powerUpGO->AddComponent<CollisionComponent>();
        cc.SetSize(16.f, 16.f);
        cc.SetResponder(std::make_unique<PowerUpCollisionResponder>(&puc));

        // Add to active scene
        if (auto* scene = SceneManager::GetInstance().GetActiveScene()) {
            scene->Add(powerUpGO);
        }
    }

} // namespace dae