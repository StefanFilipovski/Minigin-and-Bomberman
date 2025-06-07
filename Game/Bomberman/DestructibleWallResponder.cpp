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
#include <SoundIds.h>
#include <ServiceLocator.h>



namespace dae {

    DestructibleWallResponder::DestructibleWallResponder(GameObject* owner, Scene* scene)
        : m_pOwner(owner), m_pScene(scene)
    {
        m_pSheet = owner->GetComponent<SpriteSheetComponent>();
        m_pCollider = owner->GetComponent<CollisionComponent>();
    }
       
    void DestructibleWallResponder::OnCollide(GameObject* other)
    {
        if (m_Started) return;

        bool isBlast = false;
        if (!other) {
            isBlast = true;
        }
        else if (auto* otherCC = other->GetComponent<CollisionComponent>()) {
            auto* resp = otherCC->GetResponder();
            if (resp) {
                isBlast = dynamic_cast<BlastResponder*>(resp) != nullptr;
            }
        }

        if (isBlast)
        {
            m_Started = true;

            // Play animation
            if (m_pSheet) {
                m_pSheet->ChangeAnimationRow(1, 7, 0, 0.1f);
                m_pSheet->SetLoop(false);
            }

            ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_WALL_DESTROY, 0.7f);

            // Hide sprite after animation
            constexpr Uint32 hideDelayMs = 500;
            SDL_AddTimer(hideDelayMs,
                [](Uint32, void* param)->Uint32 {
                    if (param) {
                        static_cast<SpriteSheetComponent*>(param)->Hide();
                    }
                    return 0;
                },
                m_pSheet);

            // Disable collision so players can walk through
            if (m_pCollider) {
                m_pCollider->SetResponder(nullptr);
            }
        }
        else
        {
            // Block movement
            if (auto* pc = other->GetComponent<PlayerComponent>()) {
                pc->RevertMove();
                return;
            }

            if (auto* enemy = other->GetComponent<BaseEnemyComponent>()) {
                enemy->RevertMove();
                return;
            }
        }
    }

   

} // namespace dae