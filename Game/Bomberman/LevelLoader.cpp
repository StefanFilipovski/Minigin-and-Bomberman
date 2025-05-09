#include "LevelLoader.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ResourceManager.h"
#include "StaticWallResponder.h"
#include "PlayerComponent.h"
#include "Scene.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <functional>
#include "SpriteSheetComponent.h"


#include "LevelLoader.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ResourceManager.h"
#include "StaticWallResponder.h"
#include "PlayerComponent.h"
#include "SpriteSheetComponent.h"
#include "Scene.h"
#include "InputManager.h"
#include "LambdaCommand.h"
#include <SDL.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <memory>
#include "LivesDisplay.h"
#include "Camera.h"
#include "BombComponent.h"
#include "DestructibleWallResponder.h"

namespace dae {

    void LevelLoader::LoadLevel(const std::string& filename,
        const std::string& sceneName)
    {
        // Open level file
        std::ifstream file{ filename };
        if (!file.is_open()) {
            std::cerr << "LevelLoader::LoadLevel - failed to open " << filename << "\n";
            return;
        }

        // Preload textures
        auto& rm = ResourceManager::GetInstance();
        rm.LoadTexture("MetalBackground.tga");
        rm.LoadTexture("GrassBackground.tga");
        rm.LoadTexture("StaticWall.tga");
        rm.LoadTexture("BreakableWallSpritesheet.tga");
        rm.LoadTexture("BombSpritesheet.tga");
        rm.LoadTexture("RedSquare.tga");
        rm.LoadTexture("BombermanSpritesheet.tga");

        // Constants
        const float tileSize = 16.f;
        constexpr int uiRows = 4;
        const float uiOffsetY = tileSize * uiRows;

        // Create or clear scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);
        Scene* pScene = &scene;


        // Metal UI background
        {
            auto metalBg = std::make_shared<GameObject>();
            metalBg->AddComponent<TransformComponent>()
                .SetLocalPosition(0.f, 0.f, 0.f);
            metalBg->AddComponent<RenderComponent>()
                .SetTexture("MetalBackground.tga");
            scene.Add(metalBg);
        }
        // Grass world background
        {
            auto grassBg = std::make_shared<GameObject>();
            grassBg->AddComponent<TransformComponent>()
                .SetLocalPosition(0.f, uiOffsetY, 0.f);
            grassBg->AddComponent<RenderComponent>()
                .SetTexture("GrassBackground.tga");
            scene.Add(grassBg);
        }

        std::string line;
        int row = 0;
        std::shared_ptr<GameObject> playerGO;

        while (std::getline(file, line)) {
            SDL_PumpEvents();
            InputManager::GetInstance().ProcessInput();

            for (int col = 0; col < static_cast<int>(line.size()); ++col) {
                char tile = line[col];
                float x = col * tileSize;
                float y = row * tileSize + uiOffsetY;

                if (tile == 'W') {
                    auto wall = std::make_shared<GameObject>();
                    wall->AddComponent<TransformComponent>()
                        .SetLocalPosition(x, y, 0.f);
                    wall->AddComponent<RenderComponent>()
                        .SetTexture("StaticWall.tga");
                    auto& cc = wall->AddComponent<CollisionComponent>();
                    cc.SetSize(tileSize, tileSize);
                    cc.SetResponder(std::make_unique<StaticWallResponder>());
                    scene.Add(wall);
                }
                else if (tile == 'B') {
                    auto wall = std::make_shared<GameObject>();
                    wall->AddComponent<TransformComponent>()
                        .SetLocalPosition(x, y, 0.f);
                    auto& sheet = wall->AddComponent<SpriteSheetComponent>();
                    sheet.SetSpriteSheet("BreakableWallSpritesheet.tga",
                        /*rows=*/1, /*cols=*/7,
                        /*startRow=*/0, /*frameTime=*/0.1f);
                    sheet.SetIdleFrame(
                        SpriteSheetComponent::AnimationState::Idle,
                        /*rows=*/1, /*cols=*/7,
                        /*frameRow=*/0, /*frameCol=*/0
                    );
                    auto& cc = wall->AddComponent<CollisionComponent>();
                    cc.SetSize(tileSize, tileSize);
                    cc.SetResponder(std::make_unique<DestructibleWallResponder>(wall.get()));
                    scene.Add(wall);
                }
                else if (tile == 'P')
                {
                    // 1) Parent GO: movement + collision
                    auto parentGO = std::make_shared<GameObject>();
                    parentGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x, y, 0.f);
                    auto& cc = parentGO->AddComponent<CollisionComponent>();
                    const float colliderSize = tileSize * 0.8f;
                    cc.SetSize(colliderSize, colliderSize);
                    scene.Add(parentGO);

                    // 2) Child GO: just the sprite, with its pixel offset
                    auto spriteGO = std::make_shared<GameObject>();
                    constexpr float pixelOffsetX = 8.f;
                    constexpr float pixelOffsetY = 3.f;
                    spriteGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(pixelOffsetX, pixelOffsetY, 0.f);
                    auto& sprite = spriteGO->AddComponent<SpriteSheetComponent>();
                    sprite.SetSpriteSheet("BombermanSpritesheet.tga", 3, 6, 0, 0.15f);
                    sprite.SetIdleFrame(SpriteSheetComponent::AnimationState::Idle,
                        3, 6, 0, 4);

                    // Parent the sprite *before* adding the PlayerComponent
                    scene.Add(spriteGO);
                    scene.Add(parentGO);
                    spriteGO->SetParent(parentGO.get(), /*keepWorldTransform=*/true);

                    // 3) Now that the sprite exists in the hierarchy, add your PlayerComponent
                    auto& pc = parentGO->AddComponent<PlayerComponent>();
                    pc.BeginMove();
                    auto& lives = parentGO->AddComponent<LivesDisplay>(3);
                    pc.AddObserver(&lives);

                    // 4) Finally hook up the camera and input
                    Camera::GetInstance().SetTarget(parentGO);
                 
                    auto& input = InputManager::GetInstance();
                    auto bindDU = [&](int code, InputDeviceType dev, int pIdx,
                        PlayerComponent::Direction dir) {
                            input.BindCommand(code, KeyState::Down, dev,
                                std::make_unique<LambdaCommand>(
                                    [&pc, dir]() { pc.OnMovementPressed(dir); }
                                ), pIdx);
                            input.BindCommand(code, KeyState::Up, dev,
                                std::make_unique<LambdaCommand>(
                                    [&pc, dir]() { pc.OnMovementReleased(dir); }
                                ), pIdx);
                        };
                    bindDU(SDL_SCANCODE_LEFT, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Left);
                    bindDU(SDL_SCANCODE_RIGHT, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Right);
                    bindDU(SDL_SCANCODE_UP, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Up);
                    bindDU(SDL_SCANCODE_DOWN, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Down);

                    const std::string levelCopy = sceneName;

                    input.BindCommand(
                        SDL_SCANCODE_X, KeyState::Down, InputDeviceType::Keyboard,
                        std::make_unique<LambdaCommand>(
                            [parentGO, pScene]() {
                                // 1) Snap parentGO’s position (which is your player) to grid
                                constexpr float s_TileSize = 16.f;
                                auto raw = parentGO->GetTransform().GetWorldPosition();
                                glm::vec2 gridPos{
                                    std::floor(raw.x / s_TileSize + 0.5f) * s_TileSize,
                                    std::floor(raw.y / s_TileSize + 0.5f) * s_TileSize
                                };

                            // 2) Spawn bomb at grid-aligned position (no parenting)
                            auto bombGO = std::make_shared<GameObject>();
                            bombGO->AddComponent<TransformComponent>()
                                .SetLocalPosition(gridPos.x, gridPos.y, 0.f);

                            // 3) Initialize BombComponent
                            auto& bc = bombGO->AddComponent<BombComponent>();
                            bc.Init(
                                "BombSpritesheet.tga", // fuse sheet
                                /*cols=*/3, /*rows=*/1,
                                /*frameTime=*/0.2f,
                                /*range=*/1,
                                /*fuseTime=*/2.f,
                                *pScene
                            );

                            // 4) Add to scene root (no parent)
                            pScene->Add(bombGO);
                            }),
                        /*priority=*/1
                    );
                }
            }
            ++row;
        }
    }

} // namespace dae
