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
                else if (tile == 'B')
                {
                    // 1) World‐position for the tile
                    
                    // 2) Tweak these until the sprites line up visually
                    constexpr float spriteOffsetX = 8.f;
                    constexpr float spriteOffsetY = 8.f;

                    // 3) Create the wall GO & nudge its transform for the art
                    auto wallGO = std::make_shared<GameObject>();
                    wallGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffsetX,
                            y + spriteOffsetY,
                            0.f);

                    // 4) Add your spritesheet on that same GO
                    auto& sheet = wallGO->AddComponent<SpriteSheetComponent>();
                    sheet.SetSpriteSheet("BreakableWallSpritesheet.tga",
                        /*rows=*/1, /*cols=*/7,
                        /*targetRow=*/0,
                        /*frameTime=*/0.1f);
                    sheet.SetIdleFrame(
                        SpriteSheetComponent::AnimationState::Idle,
                        /*rows=*/1, /*cols=*/7,
                        /*frameRow=*/0, /*frameCol=*/0
                    );

                    // 5) Add & offset the collider back under the tile
                    auto& cc = wallGO->AddComponent<CollisionComponent>();
                    cc.SetSize(tileSize, tileSize);
                    // since collider == tileSize, baseOffset == 0
                    // we only need to counteract our spriteOffset
                    cc.SetOffset(-spriteOffsetX, -spriteOffsetY);

                    // 6) Attach the responder and add to scene
                    cc.SetResponder(std::make_unique<DestructibleWallResponder>(wallGO.get()));
                    scene.Add(wallGO);
                }
                else if (tile == 'P')
                {
                           

                    // 2) Create one GO for both sprite & collision
                    auto playerGO = std::make_shared<GameObject>();

                    //  a) nudge the whole transform so the art aligns
                    constexpr float pixelOffsetX = 7.5f;  // tweak these... positive is to the left, negative is to the right
                    constexpr float pixelOffsetY = 5.f;
                    playerGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + pixelOffsetX,
                            y + pixelOffsetY,
                            0.f);

                    //  b) add the sprite
                    auto& sprite = playerGO->AddComponent<SpriteSheetComponent>();
                    sprite.SetSpriteSheet("BombermanSpritesheet.tga",
                        /*rows=*/3, /*cols=*/6,
                        /*targetRow=*/0,
                        /*frameTime=*/0.15f);
                    sprite.SetIdleFrame(SpriteSheetComponent::AnimationState::Idle,
                        3, 6, 0, 4);

                    //  c) add & offset the collider under that sprite
                    auto& cc = playerGO->AddComponent<CollisionComponent>();
                    const float colliderSize = tileSize * 0.8f;
                    cc.SetSize(colliderSize, colliderSize+2);
                    // center it, then nudge opposite of sprite-offset:
                    float baseOffset = (tileSize - colliderSize) * 0.5f;
                    cc.SetOffset(baseOffset - pixelOffsetX,
                        baseOffset - pixelOffsetY);

                    //  d) player logic
                    auto& pc = playerGO->AddComponent<PlayerComponent>();
                    pc.BeginMove();
                    auto& lives = playerGO->AddComponent<LivesDisplay>(3);
                    pc.AddObserver(&lives);

                    // 3) Add to scene & hook camera
                    scene.Add(playerGO);
                    Camera::GetInstance().SetTarget(playerGO);

                    // 4) Bind input (use playerGO’s PlayerComponent)
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

                    // 5) Bomb binding (capture playerGO, not parentGO)
                    input.BindCommand(
                        SDL_SCANCODE_X, KeyState::Down, InputDeviceType::Keyboard,
                        std::make_unique<LambdaCommand>(
                            [&pc, pScene]() { pc.PlaceBomb(*pScene); }
                        ),
                        /*priority=*/1
                    );
                }
            }
            ++row;
        }
    }

} // namespace dae
