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
        if (m_MarkedForCleanup) return;  // Skip updates if marked for cleanup

        if (m_State)
            m_State->Update(*this, dt);
    }

    void BombComponent::Explode()
    {
        // Prevent multiple explosions
        if (m_Exploded || m_MarkedForCleanup || m_IsExploding) return;
        m_Exploded = true;
        m_IsExploding = true;

        // Safety check - ensure our owner is valid
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            m_IsExploding = false;
            return;
        }

        // Hide sprite immediately
        if (m_pSprite) {
            m_pSprite->Hide();
        }

        // Disable our collision immediately
        if (auto* cc = GetOwner()->GetComponent<CollisionComponent>()) {
            cc->SetSize(0, 0);  // Make it 0 size
            cc->SetResponder(nullptr);
        }

        ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_BOMB_EXPLODE, 1.0f);

        // Get transform using the SAFE method
        auto* transform = GetOwner()->GetTransformSafe();
        if (!transform) {
            std::cerr << "BombComponent::Explode - Bomb has no transform!" << std::endl;
            m_IsExploding = false;
            return;
        }

        // Calculate explosion center
        constexpr float bombVisOffsetX = 8.f;
        constexpr float bombVisOffsetY = 8.f;
        auto world3 = transform->GetWorldPosition();
        world3.x -= bombVisOffsetX;
        world3.y -= bombVisOffsetY;

        glm::vec2 center{
            std::floor(world3.x / s_TileSize + 0.5f) * s_TileSize,
            std::floor(world3.y / s_TileSize + 0.5f) * s_TileSize
        };

        // Clear any existing blasts
        m_Blasts.clear();
        m_Blasts.reserve(1 + 4 * m_Range);

        // Lambda to spawn blast - ensure transform is added first
        auto spawnBlastAt = [&](const glm::vec2& pos, BlastResponder::Segment seg) {
            try {
                auto blast = std::make_shared<GameObject>();

                // CRITICAL: Add TransformComponent first
                blast->AddComponent<TransformComponent>().SetLocalPosition(pos.x, pos.y, 0.f);

                // Then add render component
                blast->AddComponent<RenderComponent>().SetTexture("RedSquare.tga");

                // Finally add collision
                auto& cc = blast->AddComponent<CollisionComponent>();
                cc.SetSize(s_TileSize, s_TileSize);
                cc.SetResponder(std::make_unique<BlastResponder>(seg));

                // Add to scene and track
                m_pScene->Add(blast);
                m_Blasts.push_back(blast);
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to spawn blast: " << e.what() << std::endl;
            }
            };

        // Spawn center blast
        spawnBlastAt(center, BlastResponder::Segment::Center);

        // Structure to store walls to destroy
        struct WallToDestroy {
            GameObject* owner;
            DestructibleWallResponder* responder;
        };
        std::vector<WallToDestroy> wallsToDestroy;

        // Spawn directional blasts
        constexpr glm::vec2 dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        for (auto dir : dirs)
        {
            glm::vec2 cursor = center;
            for (int i = 1; i <= m_Range; ++i)
            {
                cursor += s_TileSize * dir;
                SDL_Rect box{ int(cursor.x), int(cursor.y),
                              int(s_TileSize), int(s_TileSize) };

                bool hitStatic = false;
                GameObject* hitObject = nullptr;
                DestructibleWallResponder* destructibleHit = nullptr;

                // Make a copy of components to avoid iterator issues
                auto components = CollisionManager::GetInstance().GetComponents();

                for (auto* c : components)
                {
                    if (!c || !c->GetOwner()) continue;
                    if (c->GetOwner()->IsMarkedForDeletion()) continue;

                    auto* resp = c->GetResponder();
                    if (!resp) continue;

                    SDL_Rect cBox = c->GetBoundingBox();
                    // Check if bounding box is valid (not empty)
                    if (cBox.w == 0 || cBox.h == 0) continue;

                    // Check for walls
                    if (dynamic_cast<StaticWallResponder*>(resp))
                    {
                        if (AABBIntersect(box, cBox))
                        {
                            hitStatic = true;
                            break;
                        }
                    }
                    else if (auto* dwr = dynamic_cast<DestructibleWallResponder*>(resp))
                    {
                        if (AABBIntersect(box, cBox))
                        {
                            hitObject = c->GetOwner();
                            destructibleHit = dwr;
                            break;
                        }
                    }
                }

                // If hit static wall, stop
                if (hitStatic)
                    break;

                // If hit destructible wall, mark for destruction
                if (destructibleHit && hitObject)
                {
                    wallsToDestroy.push_back({ hitObject, destructibleHit });
                    // Place blast on the wall
                    spawnBlastAt(cursor, BlastResponder::Segment::End);
                    break;  // Stop after destructible wall
                }

                // No wall hit, place blast
                auto seg = (i == m_Range ? BlastResponder::Segment::End : BlastResponder::Segment::Middle);
                spawnBlastAt(cursor, seg);
            }
        }

        // Schedule wall destruction for next frame to avoid iterator issues
        if (!wallsToDestroy.empty()) {
            SDL_AddTimer(1, // 1ms delay
                [](Uint32, void* param) -> Uint32 {
                    auto* walls = static_cast<std::vector<WallToDestroy>*>(param);
                    for (const auto& wall : *walls) {
                        if (wall.owner && !wall.owner->IsMarkedForDeletion() && wall.responder) {
                            wall.responder->OnCollide(nullptr);
                        }
                    }
                    delete walls;
                    return 0;
                },
                new std::vector<WallToDestroy>(wallsToDestroy)
            );
        }

        // Transition to hide state
        TransitionTo(new BombHideState());

        // Notify AFTER everything is set up
        Event bombEvent{ GameEvents::BOMB_EXPLODED };
        Notify(bombEvent);

        m_IsExploding = false;
    }

    void BombComponent::ForceExplode()
    {
        if (!m_Exploded && !m_IsExploding && m_State) {
            // Transition directly to explosion
            Explode();
        }
    }
}