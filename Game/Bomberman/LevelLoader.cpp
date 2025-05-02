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

namespace dae
{
    void LevelLoader::LoadLevel(const std::string& filename,
        const std::string& sceneName)
    {
        std::ifstream file{ filename };
        if (!file.is_open())
        {
            std::cerr << "LevelLoader::LoadLevel - failed to open "
                << filename << "\n";
            return;
        }

        // ——— Pre-load textures ONCE
        auto& rm = ResourceManager::GetInstance();
        rm.LoadTexture("StaticWall.tga");
        rm.LoadTexture("BombermanSpritesheet.tga");  // only spritesheet for player

        // ——— Create (or clear+get) the scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        const float tileSize = 16.f;
        std::string line;
        int row = 0;

        while (std::getline(file, line))
        {
            // ←— keep the OS event queue flowing
            SDL_PumpEvents();
            InputManager::GetInstance().ProcessInput();

            for (int col = 0; col < static_cast<int>(line.size()); ++col)
            {
                char tile = line[col];
                float x = col * tileSize;
                float y = row * tileSize;

                if (tile == 'W')
                {
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
                else if (tile == 'P')
                {
                    auto player = std::make_shared<GameObject>();

                    // Position
                    player->AddComponent<TransformComponent>()
                        .SetLocalPosition(x, y, 0.f);

                    // Sprite-sheet only
                    auto& sprite = player->AddComponent<SpriteSheetComponent>();
                    sprite.SetSpriteSheet(
                        "BombermanSpritesheet.tga",
                        3, 6,    // cols, rows
                        0,       // startRow
                        0.15f    // frameTime
                    );
                    sprite.SetIdleFrame(
                        SpriteSheetComponent::AnimationState::Idle,
                        3, 6,    // sheet dims
                        0, 4     // frame row=0, col=4
                    );

                    // Collision: same size you had, but now centered
                    auto& cc = player->AddComponent<CollisionComponent>();
                    const float w = tileSize;       // your width
                    const float h = tileSize + 3;   // your height
                    cc.SetSize(w, h);

                    // compute offsets so the box is centered in the 16×16 tile
                    const float offsetX = (tileSize - w) * 0.5f;    // likely 0 here
                    const float offsetY = (tileSize - h) * 0.5f;    // negative if h>tileSize
                    cc.SetOffset(offsetX-8, offsetY-9);

                    // Gameplay components
                    auto& pc = player->AddComponent<PlayerComponent>();
                    auto& lives = player->AddComponent<LivesDisplay>(3);
                    pc.AddObserver(&lives);

                    scene.Add(player);

                    // after you scene.Add(player);
                    dae::Camera::GetInstance().SetTarget(player);

                    // Bind input now that PlayerComponent exists
                    auto& input = InputManager::GetInstance();
                    auto bindDU = [&](int code,
                        InputDeviceType dev,
                        int pIdx,
                        PlayerComponent::Direction dir)
                        {
                            input.BindCommand(
                                code, KeyState::Down, dev,
                                std::make_unique<LambdaCommand>(
                                    [&pc, dir]() { pc.OnMovementPressed(dir); }
                                ), pIdx
                            );
                            input.BindCommand(
                                code, KeyState::Up, dev,
                                std::make_unique<LambdaCommand>(
                                    [&pc, dir]() { pc.OnMovementReleased(dir); }
                                ), pIdx
                            );
                        };

                    bindDU(SDL_SCANCODE_LEFT, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Left);
                    bindDU(SDL_SCANCODE_RIGHT, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Right);
                    bindDU(SDL_SCANCODE_UP, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Up);
                    bindDU(SDL_SCANCODE_DOWN, InputDeviceType::Keyboard, 1, PlayerComponent::Direction::Down);
                }
            }
            ++row;
        }
    }
}
