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
#include "InputManager.h"
#include "LambdaCommand.h"
#include <SDL.h>
#include <memory>
#include "LivesDisplay.h"
#include "Camera.h"
#include "BombComponent.h"
#include "DestructibleWallResponder.h"
#include "MoveDirCommand.h"
#include "BombCommand.h"
#include "BalloonComponent.h"
#include "EnemyCollisionResponder.h" 
#include "OnealComponent.h"
#include "PlayerManager.h"  // Add this include
#include "DollComponent.h"

namespace dae {

    void LevelLoader::LoadLevel(const std::string& filename,
        const std::string& sceneName)
    {
        // 1) Read entire level file into memory
        std::ifstream file{ filename };
        if (!file.is_open()) {
            std::cerr << "LevelLoader::LoadLevel - failed to open " << filename << "\n";
            return;
        }
        std::vector<std::string> mapRows;
        std::string line;
        while (std::getline(file, line)) {
            mapRows.push_back(line);
        }
        file.close();

        // 2) Preload textures
        auto& rm = ResourceManager::GetInstance();
        rm.LoadTexture("MetalBackground.tga");
        rm.LoadTexture("GrassBackground.tga");
        rm.LoadTexture("StaticWall.tga");
        rm.LoadTexture("BreakableWallSpritesheet.tga");
        rm.LoadTexture("BombSpritesheet.tga");
        rm.LoadTexture("RedSquare.tga");
        rm.LoadTexture("BombermanSpritesheet.tga");
        rm.LoadTexture("BalloomSpritesheet.tga");
        rm.LoadTexture("OnealSpritesheet.tga");

        // 3) Constants and grid dimensions
        const float tileSize = 16.f;
        constexpr int uiRows = 4;
        const float uiOffsetY = tileSize * uiRows;
        int rows = static_cast<int>(mapRows.size());
        int cols = rows > 0 ? static_cast<int>(mapRows[0].size()) : 0;

        // 4) Build walkability grid: true=free, false=blocked
        std::vector<std::vector<bool>> walkable(rows, std::vector<bool>(cols, true));
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                char t = mapRows[r][c];
                if (t == 'W' || t == 'B') walkable[r][c] = false;
            }
        }

        // 5) Create or clear scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        // 6) Add UI & background
        {
            auto metalBg = std::make_shared<GameObject>();
            metalBg->AddComponent<TransformComponent>().SetLocalPosition(0.f, 0.f, 0.f);
            metalBg->AddComponent<RenderComponent>().SetTexture("MetalBackground.tga");
            scene.Add(metalBg);
        }
        {
            auto grassBg = std::make_shared<GameObject>();
            grassBg->AddComponent<TransformComponent>().SetLocalPosition(0.f, uiOffsetY, 0.f);
            grassBg->AddComponent<RenderComponent>().SetTexture("GrassBackground.tga");
            scene.Add(grassBg);
        }

        //static ScoreObserver scoreObserver; // tracks kills & score

        // 7) Spawn map objects
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                char tile = mapRows[r][c];
                float x = c * tileSize;
                float y = r * tileSize + uiOffsetY;

                switch (tile) {
                case 'W': {
                    auto wall = std::make_shared<GameObject>();
                    wall->AddComponent<TransformComponent>().SetLocalPosition(x, y, 0.f);
                    wall->AddComponent<RenderComponent>().SetTexture("StaticWall.tga");
                    auto& cc = wall->AddComponent<CollisionComponent>();
                    cc.SetSize(tileSize, tileSize);
                    cc.SetResponder(std::make_unique<StaticWallResponder>());
                    scene.Add(wall);
                    break;
                }
                case 'B': {
                    constexpr float spriteOffset = 8.f;
                    auto brick = std::make_shared<GameObject>();
                    brick->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffset,
                            y + spriteOffset,
                            0.f);
                    auto& sheet = brick->AddComponent<SpriteSheetComponent>();
                    sheet.SetSpriteSheet("BreakableWallSpritesheet.tga", 1, 7, 0, 0.1f);
                    sheet.SetIdleFrame(
                        SpriteSheetComponent::AnimationState::Idle,
                        1, 7, 0, 0);
                    auto& cc = brick->AddComponent<CollisionComponent>();
                    cc.SetSize(tileSize, tileSize);
                    cc.SetOffset(-spriteOffset, -spriteOffset);
                    cc.SetResponder(std::make_unique<DestructibleWallResponder>(brick.get()));
                    scene.Add(brick);
                    break;
                }
                case 'P': {
                    constexpr float pxOff = 7.5f, pyOff = 5.f;
                    auto playerGO = std::make_shared<GameObject>();
                    playerGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + pxOff, y + pyOff, 0.f);
                    auto& spr = playerGO->AddComponent<SpriteSheetComponent>();
                    spr.SetSpriteSheet("BombermanSpritesheet.tga", 3, 6, 0, 0.15f);
                    spr.SetIdleFrame(
                        SpriteSheetComponent::AnimationState::Idle, 3, 6, 0, 4);
                    auto& pcc = playerGO->AddComponent<CollisionComponent>();
                    float cSize = tileSize * 0.8f;
                    pcc.SetSize(cSize, cSize + 2);
                    float off = (tileSize - cSize) * 0.5f;
                    pcc.SetOffset(off - pxOff, off - pyOff);
                    auto& pc = playerGO->AddComponent<PlayerComponent>();
                    pc.BeginMove();
                    auto& lives = playerGO->AddComponent<LivesDisplay>(3);
                    pc.AddObserver(&lives);

                    // Register player with PlayerManager
                    constexpr int pid = 0;
                    PlayerManager::GetInstance().RegisterPlayer(playerGO.get(), pid);

                    scene.Add(playerGO);
                    Camera::GetInstance().SetTarget(playerGO);
                    auto& input = InputManager::GetInstance();
                    auto bindKey = [&](SDL_Scancode k, PlayerComponent::Direction d) {
                        input.BindCommand(k, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<MoveDirCommand>(&pc, d, true), pid);
                        input.BindCommand(k, KeyState::Up, InputDeviceType::Keyboard,
                            std::make_unique<MoveDirCommand>(&pc, d, false), pid);
                        };
                    bindKey(SDL_SCANCODE_LEFT, PlayerComponent::Direction::Left);
                    bindKey(SDL_SCANCODE_RIGHT, PlayerComponent::Direction::Right);
                    bindKey(SDL_SCANCODE_UP, PlayerComponent::Direction::Up);
                    bindKey(SDL_SCANCODE_DOWN, PlayerComponent::Direction::Down);
                    input.BindCommand(
                        SDL_SCANCODE_X, KeyState::Down, InputDeviceType::Keyboard,
                        std::make_unique<BombCommand>(&pc, &scene), pid);
                    break;
                }
                case 'O': { // Oneal enemy
                    constexpr float spriteOffset = 8.f;
                    auto enemyGO = std::make_shared<GameObject>();
                    enemyGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffset, y + spriteOffset, 0.f);

                    enemyGO->AddComponent<SpriteSheetComponent>()
                        .SetSpriteSheet("OnealSpritesheet.tga", 1, 7, 0, 0.2f);

                    auto& oc = enemyGO->AddComponent<OnealComponent>(
                        20.f,     // faster speed than balloon (13.f)
                        0.8f,     // slightly faster move interval
                        100.f,     // chase range in world units
                        walkable, glm::ivec2(cols, rows),
                        tileSize, uiOffsetY);

                    // No need to set player target anymore - OnealComponent uses PlayerManager

                    auto& cc = enemyGO->AddComponent<CollisionComponent>();
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<EnemyCollisionResponder>(&oc));

                    scene.Add(enemyGO);
                    break;
                }
                case 'D': { // Doll enemy
                    constexpr float spriteOffset = 8.f;
                    auto enemyGO = std::make_shared<GameObject>();
                    enemyGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffset, y + spriteOffset, 0.f);

                    enemyGO->AddComponent<SpriteSheetComponent>()
                        .SetSpriteSheet("DollSpritesheet.tga", 1, 11, 0, 0.2f);

                    auto& dc = enemyGO->AddComponent<DollComponent>(
                        18.f,     // faster than balloon (13.f) but slower than Oneal (20.f)
                        0.9f,     // slightly faster decision making than balloon
                        walkable, glm::ivec2(cols, rows),
                        tileSize, uiOffsetY);

                    auto& cc = enemyGO->AddComponent<CollisionComponent>();
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<EnemyCollisionResponder>(&dc));

                    scene.Add(enemyGO);
                    break;
                }
                case 'E': {
                    constexpr float spriteOffset = 8.f;
                    auto enemyGO = std::make_shared<GameObject>();
                    enemyGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffset, y + spriteOffset, 0.f);

                    enemyGO->AddComponent<SpriteSheetComponent>()
                        .SetSpriteSheet("BalloomSpritesheet.tga", 1, 11, 0, 0.2f);

                    auto& bc = enemyGO->AddComponent<BalloonComponent>(
                        13.f, 1.f,
                        walkable, glm::ivec2(cols, rows),
                        tileSize, uiOffsetY);

                    auto& cc = enemyGO->AddComponent<CollisionComponent>();
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<EnemyCollisionResponder>(&bc));

                    scene.Add(enemyGO);
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

} // namespace dae