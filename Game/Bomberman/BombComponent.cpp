#include "BombComponent.h"
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

namespace dae {

    BombComponent::BombComponent(GameObject* owner)
        : Component(owner)
    {
    }

    void BombComponent::Init(const std::string& spriteSheet,
        int cols, int rows, float frameTime,
        int range, float fuseTime,
        Scene& scene)
    {
        m_FuseTime = fuseTime;
        m_Range = range;
        m_pScene = &scene;

        m_pSprite = &GetOwner()->AddComponent<SpriteSheetComponent>();
        m_pSprite->SetSpriteSheet(spriteSheet, rows, cols, 0, frameTime);
        m_pSprite->SetLoop(true);
    }

    void BombComponent::Update(float dt)
    {
        if (!m_Exploded) {
            // fuse counting down
            m_Timer += dt;
            if (m_Timer >= m_FuseTime) {
                Explode();
                m_Exploded = true;
                m_pSprite->Hide();  // hide the bomb itself
            }
        }
        else if (m_HidePending) {
            // track time since explosion to hide blasts
            m_HideTimer += dt;
            if (m_HideTimer >= 1.f) {
                for (auto& blast : m_Blasts) {
                    if (!blast) continue;
                    if (auto* rc = blast->GetComponent<RenderComponent>())
                        rc->SetScale(0.f, 0.f);            // collapse it out of view
                    if (auto* cc = blast->GetComponent<CollisionComponent>())
                        cc->SetResponder(nullptr);         // disable further collision

                }
                m_HidePending = false;
                m_Blasts.clear();
            }
        }
    }

    void BombComponent::Explode()
    {
        // 1) compute the grid-aligned center
        auto world3 = GetOwner()->GetTransform().GetWorldPosition();
        glm::vec2 center{
            std::floor(world3.x / s_TileSize + 0.5f) * s_TileSize,
            std::floor(world3.y / s_TileSize + 0.5f) * s_TileSize
        };

        // 2) clear any previous blasts and reserve space
        m_Blasts.clear();
        m_Blasts.reserve(1 + 4 * m_Range);

        // 3) helper: spawn and record a blast tile
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

        // 4) always draw the center piece
        spawnBlastAt(center, BlastResponder::Segment::Center);

        // 5) cast in four directions
        constexpr glm::vec2 dirs[4] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
        for (auto dir : dirs)
        {
            glm::vec2 cursor = center;
            for (int i = 1; i <= m_Range; ++i)
            {
                cursor += dir * s_TileSize;
                SDL_Rect box{
                    int(cursor.x), int(cursor.y),
                    int(s_TileSize), int(s_TileSize)
                };

                // only test wall responders
                CollisionComponent* hit = nullptr;
                for (auto* c : CollisionManager::GetInstance().GetComponents())
                {
                    auto* resp = c->GetResponder();
                    if (!dynamic_cast<StaticWallResponder*>(resp) &&
                        !dynamic_cast<DestructibleWallResponder*>(resp))
                        continue;
                    if (AABBIntersect(box, c->GetBoundingBox()))
                    {
                        hit = c;
                        break;
                    }
                }

                // static wall → stop, no tile
                if (hit && dynamic_cast<StaticWallResponder*>(hit->GetResponder()))
                    break;

                // destructible → trigger crumble, then stop (no tile on wall)
                if (hit)
                {
                    if (auto* d = dynamic_cast<DestructibleWallResponder*>(hit->GetResponder()))
                        d->OnCollide(nullptr);
                    break;
                }

                // empty → spawn middle or end piece
                auto seg = (i == m_Range
                    ? BlastResponder::Segment::End
                    : BlastResponder::Segment::Middle);
                spawnBlastAt(cursor, seg);
            }
        }

        // 6) start the hide timer (hiding handled in Update())
        m_HidePending = true;
        m_HideTimer = 0.f;

        // 7) optional: dispatch any remaining collisions
        CollisionManager::GetInstance().CheckCollisions();
    }




} // namespace dae
