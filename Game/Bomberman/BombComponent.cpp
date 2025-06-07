// BombComponent.cpp
#include "BombComponent.h"

// state classes
#include "BombFuseState.h"
#include "BombHideState.h"

// your existing headers
#include "SpriteSheetComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "CollisionUtilities.h"
#include "StaticWallResponder.h"
#include "DestructibleWallResponder.h"
#include "SceneManager.h"
#include "Scene.h"
#include <SDL.h>
#include "BlastResponder.h"
#include <SoundIds.h>
#include <ServiceLocator.h>

namespace dae
{
    BombComponent::BombComponent(GameObject* owner)
        : Component(owner)
    {
    }

    void BombComponent::Init(const std::string& spriteSheet,
        int cols, int rows, float frameTime,
        int range, float fuseTime,
        Scene& scene)
    {
        // store scene & parameters
        m_pScene = &scene;
        m_Range = range;
        m_FuseTime = fuseTime;

        // set up sprite
        m_pSprite = &GetOwner()->AddComponent<SpriteSheetComponent>();
        m_pSprite->SetSpriteSheet(spriteSheet, rows, cols, 0, frameTime);
        m_pSprite->SetLoop(true);

        // start in the fuse state
        TransitionTo(new BombFuseState());
    }

    void BombComponent::TransitionTo(IBombState* newState)
    {
        m_State.reset(newState);
    }

  

    void BombComponent::Update(float dt)
    {
        if (m_State)
            m_State->Update(*this, dt);
    }

    void BombComponent::Explode()
    {
        if (m_Exploded) return;
        m_Exploded = true;

        if (m_pSprite) {
            m_pSprite->Hide();
        }

        ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_BOMB_EXPLODE, 1.0f);

        // Notify observers that this bomb has exploded
        Event bombEvent{ GameEvents::BOMB_EXPLODED };
        Notify(bombEvent);

        // 0) undo the visual offset you applied at placement
        constexpr float bombVisOffsetX = 8.f;  // must match your placement
        constexpr float bombVisOffsetY = 8.f;

        // 1) read the world‐position (currently includes visual offset)
        auto world3 = GetOwner()->GetTransform().GetWorldPosition();
        // 2) subtract it to get back to grid‐space
        world3.x -= bombVisOffsetX;
        world3.y -= bombVisOffsetY;

        // 3) compute the grid‐aligned center
        glm::vec2 center{
          std::floor(world3.x / s_TileSize + 0.5f) * s_TileSize,
          std::floor(world3.y / s_TileSize + 0.5f) * s_TileSize
        };

        // … the rest of your existing code stays exactly the same …
        m_Blasts.clear();
        m_Blasts.reserve(1 + 4 * m_Range);

        auto spawnBlastAt = [&](const glm::vec2& pos, BlastResponder::Segment seg) {
            auto blast = std::make_shared<GameObject>();
            blast->AddComponent<TransformComponent>()
                .SetLocalPosition(pos.x, pos.y, 0.f);
            auto& rc = blast->AddComponent<RenderComponent>();
            rc.SetTexture("RedSquare.tga");
            auto& cc = blast->AddComponent<CollisionComponent>();
            cc.SetSize(s_TileSize, s_TileSize);
            cc.SetResponder(std::make_unique<BlastResponder>(seg));
            m_pScene->Add(blast);
            m_Blasts.push_back(blast);
            return blast;
            };

        spawnBlastAt(center, BlastResponder::Segment::Center);

        constexpr glm::vec2 dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        for (auto dir : dirs)
        {
            glm::vec2 cursor = center;
            for (int i = 1; i <= m_Range; ++i)
            {
                cursor += s_TileSize * dir;
                SDL_Rect box{ int(cursor.x), int(cursor.y),
                              int(s_TileSize), int(s_TileSize) };
                CollisionComponent* hit = nullptr;
                for (auto* c : CollisionManager::GetInstance().GetComponents())
                {
                    auto* resp = c->GetResponder();

                    // Check if responder is null (destroyed wall)
                    if (!resp) {
                        continue;
                    }

                    if (!dynamic_cast<StaticWallResponder*>(resp) &&
                        !dynamic_cast<DestructibleWallResponder*>(resp))
                        continue;

                    if (AABBIntersect(box, c->GetBoundingBox()))
                    {
                        hit = c;
                        break;
                    }
                }

                if (hit && hit->GetResponder() && dynamic_cast<StaticWallResponder*>(hit->GetResponder()))
                    break;

                if (hit && hit->GetResponder())
                {
                    if (auto* d = dynamic_cast<DestructibleWallResponder*>(hit->GetResponder()))
                        d->OnCollide(nullptr);
                    break;
                }

                auto seg = (i == m_Range
                    ? BlastResponder::Segment::End
                    : BlastResponder::Segment::Middle);
                spawnBlastAt(cursor, seg);
            }
        }
        

        TransitionTo(new BombHideState());
        CollisionManager::GetInstance().CheckCollisions();
    }

    void BombComponent::ForceExplode()
    {
        if (!m_Exploded) {
            Explode();  // This should hide the sprite
        }
    }

}
