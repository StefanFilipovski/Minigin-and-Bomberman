#include "CoopGameLoader.h"
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
#include "Camera.h"
#include "BombComponent.h"
#include "DestructibleWallResponder.h"
#include "MoveDirCommand.h"
#include "BombCommand.h"
#include "BalloonComponent.h"
#include "EnemyCollisionResponder.h" 
#include "OnealComponent.h"
#include "PlayerManager.h"  
#include "DollComponent.h"
#include "MinvoComponent.h"
#include "PowerUpType.h"
#include "PowerUpComponent.h"
#include "PowerUpCollisionResponder.h"
#include "DetonatorCommand.h"
#include "EnemyManager.h"
#include "CoopGameController.h"
#include "ServiceLocator.h"
#include "MuteCommand.h"
#include "ScoreComponent.h"
#include "ScoreManager.h"
#include "LevelSkipCommand.h"
#include "CollisionManager.h"
#include "LivesDisplayComponent.h"
#include <random>

namespace dae {

    void CoopGameLoader::LoadCoopGame(const std::string& filename, const std::string& sceneName)
    {
        std::cout << "Loading co-op game..." << std::endl;

        // IMPORTANT: Clear everything properly to avoid lag
        EnemyManager::GetInstance().ClearLevel();
        PlayerManager::GetInstance().ClearPlayers();
        CollisionManager::GetInstance().Clear();

        // Clear input bindings
        InputManager::GetInstance().ClearAllBindings();

        // 1) Read entire level file into memory
        std::ifstream file{ filename };
        if (!file.is_open()) {
            std::cerr << "CoopGameLoader::LoadCoopGame - failed to open " << filename << "\n";
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
        rm.LoadTexture("MinvoSpritesheet.tga");
        rm.LoadTexture("PowerUpBomb.tga");
        rm.LoadTexture("PowerUpDetonator.tga");
        rm.LoadTexture("PowerUpFlame.tga");
        rm.LoadTexture("Exit.tga");
        rm.LoadTexture("DollSpritesheet.tga");

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

        EnemyManager::GetInstance().SetCurrentScene(&scene);

        ServiceLocator::GetSoundSystem().PlayMusic("Bomberman (NES) Music - Stage Theme.ogg", 0.7f);

        // Add co-op game controller object
        auto gameController = std::make_shared<GameObject>();
        auto& controller = gameController->AddComponent<CoopGameController>();
        scene.Add(gameController);

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

        auto scoreGO = std::make_shared<GameObject>();
        scoreGO->AddComponent<TransformComponent>().SetLocalPosition(10.f, 10.f, 0.f);
        auto& scoreComp = scoreGO->AddComponent<ScoreComponent>();
        scoreComp.Initialize();
        scene.Add(scoreGO);

        // Track if we've found the player spawn point for player positioning
        bool foundPlayerSpawn = false;
        std::shared_ptr<GameObject> livesDisplayGO = nullptr;

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

                    // Random power-up generation (same as story mode)
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    static std::uniform_real_distribution<> chance(0.0, 1.0);
                    static std::uniform_int_distribution<> typeGen(0, 2);

                    // 30% chance for a power-up behind this wall
                    if (chance(gen) <= 0.3f) {
                        auto powerUp = std::make_shared<GameObject>();
                        powerUp->AddComponent<TransformComponent>()
                            .SetLocalPosition(x, y, 0.f);

                        PowerUpType type = static_cast<PowerUpType>(typeGen(gen));

                        auto& rc = powerUp->AddComponent<RenderComponent>();
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

                        auto& puc = powerUp->AddComponent<PowerUpComponent>(type);

                        auto& cc = powerUp->AddComponent<CollisionComponent>();
                        float powerUpCollisionSize = tileSize * 0.6f;
                        cc.SetSize(powerUpCollisionSize, powerUpCollisionSize);
                        float collisionOffset = (tileSize - powerUpCollisionSize) * 0.5f;
                        cc.SetOffset(collisionOffset, collisionOffset);
                        cc.SetResponder(std::make_unique<PowerUpCollisionResponder>(&puc));

                        scene.Add(powerUp);
                    }

                    // Create the destructible wall on top
                    auto brick = std::make_shared<GameObject>();
                    brick->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffset, y + spriteOffset, 0.f);

                    auto& sheet = brick->AddComponent<SpriteSheetComponent>();
                    sheet.SetSpriteSheet("BreakableWallSpritesheet.tga", 1, 7, 0, 0.1f);
                    sheet.SetIdleFrame(SpriteSheetComponent::AnimationState::Idle, 1, 7, 0, 0);

                    auto& cc = brick->AddComponent<CollisionComponent>();
                    cc.SetSize(tileSize, tileSize);
                    cc.SetOffset(-spriteOffset, -spriteOffset);
                    cc.SetResponder(std::make_unique<DestructibleWallResponder>(brick.get(), &scene));

                    scene.Add(brick);
                    break;
                }
                case 'M': { // Minvo enemy
                    constexpr float spriteOffset = 8.f;
                    auto enemyGO = std::make_shared<GameObject>();
                    enemyGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffset, y + spriteOffset, 0.f);

                    enemyGO->AddComponent<SpriteSheetComponent>()
                        .SetSpriteSheet("MinvoSpritesheet.tga", 1, 7, 0, 0.2f);

                    auto& mc = enemyGO->AddComponent<MinvoComponent>(
                        25.f, 0.6f, 144.f,
                        walkable, glm::ivec2(cols, rows),
                        tileSize, uiOffsetY);
                    EnemyManager::GetInstance().RegisterEnemy(&mc);
                    // Also register with CoopGameController for exit spawning
                    mc.AddObserver(&controller);

                    auto& cc = enemyGO->AddComponent<CollisionComponent>();
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<EnemyCollisionResponder>(&mc));
                    mc.AddObserver(&ScoreManager::GetInstance());

                    scene.Add(enemyGO);
                    break;
                }
                case 'P': {
                    // Only create players on the first 'P' found to avoid duplicates
                    if (!foundPlayerSpawn) {
                        foundPlayerSpawn = true;

                        // Create Player 1 (same as story mode)
                        constexpr float pxOff = 7.5f, pyOff = 5.f;
                        auto player1GO = std::make_shared<GameObject>();
                        player1GO->AddComponent<TransformComponent>()
                            .SetLocalPosition(x + pxOff, y + pyOff, 0.f);

                        auto& spr1 = player1GO->AddComponent<SpriteSheetComponent>();
                        spr1.SetSpriteSheet("BombermanSpritesheet.tga", 3, 6, 0, 0.15f);
                        spr1.SetIdleFrame(SpriteSheetComponent::AnimationState::Idle, 3, 6, 0, 4);

                        auto& pcc1 = player1GO->AddComponent<CollisionComponent>();
                        float cSize = tileSize * 0.8f;
                        pcc1.SetSize(cSize, cSize + 2);
                        float off = (tileSize - cSize) * 0.5f;
                        pcc1.SetOffset(off - pxOff, off - pyOff);

                        auto& pc1 = player1GO->AddComponent<PlayerComponent>();
                        pc1.BeginMove();
                        pc1.SetLives(3); // Shared lives will be managed by controller

                        // Make player 1 observe the co-op controller
                        pc1.AddObserver(&controller);

                        // Create Lives Display GameObject (top right) if not already created
                        if (!livesDisplayGO) {
                            const float screenWidth = 256.f;
                            const float livesTextWidth = 80.f;
                            livesDisplayGO = std::make_shared<GameObject>();
                            livesDisplayGO->AddComponent<TransformComponent>().SetLocalPosition(
                                screenWidth - livesTextWidth + 50.f,
                                10.f,
                                0.f
                            );

                            // Setup Lives Display Component - start with 3 lives for co-op
                            auto& livesDisplay = livesDisplayGO->AddComponent<LivesDisplayComponent>(3);
                            livesDisplay.Initialize();

                            // Make both players observe the lives display (for hit/death events)
                            pc1.AddObserver(&livesDisplay);

                            scene.Add(livesDisplayGO);
                        }

                        // Register player 1 with PlayerManager
                        PlayerManager::GetInstance().RegisterPlayer(player1GO.get(), 0);
                        scene.Add(player1GO);

                        // Set camera target to player 1 (could be changed to follow both)
                        Camera::GetInstance().SetTarget(player1GO);

                        // Player 1 controls (Arrow keys + X + C)
                        auto& input = InputManager::GetInstance();

                        auto bindKey1 = [&](SDL_Scancode k, PlayerComponent::Direction d) {
                            input.BindCommand(k, KeyState::Down, InputDeviceType::Keyboard,
                                std::make_unique<MoveDirCommand>(&pc1, d, true), 0);
                            input.BindCommand(k, KeyState::Up, InputDeviceType::Keyboard,
                                std::make_unique<MoveDirCommand>(&pc1, d, false), 0);
                            };
                        bindKey1(SDL_SCANCODE_LEFT, PlayerComponent::Direction::Left);
                        bindKey1(SDL_SCANCODE_RIGHT, PlayerComponent::Direction::Right);
                        bindKey1(SDL_SCANCODE_UP, PlayerComponent::Direction::Up);
                        bindKey1(SDL_SCANCODE_DOWN, PlayerComponent::Direction::Down);

                        input.BindCommand(SDL_SCANCODE_X, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<BombCommand>(&pc1, &scene), 0);
                        input.BindCommand(SDL_SCANCODE_C, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<DetonatorCommand>(&pc1), 0);

                        // Create Player 2 (offset slightly to the right)
                        float player2X = x + tileSize; // One tile to the right
                        float player2Y = y;

                        auto player2GO = std::make_shared<GameObject>();
                        player2GO->AddComponent<TransformComponent>()
                            .SetLocalPosition(player2X + pxOff, player2Y + pyOff, 0.f);

                        auto& spr2 = player2GO->AddComponent<SpriteSheetComponent>();
                        spr2.SetSpriteSheet("BombermanSpritesheet.tga", 3, 6, 0, 0.15f);
                        spr2.SetIdleFrame(SpriteSheetComponent::AnimationState::Idle, 3, 6, 0, 4);

                        auto& pcc2 = player2GO->AddComponent<CollisionComponent>();
                        pcc2.SetSize(cSize, cSize + 2);
                        pcc2.SetOffset(off - pxOff, off - pyOff);

                        auto& pc2 = player2GO->AddComponent<PlayerComponent>();
                        pc2.BeginMove();
                        pc2.SetLives(3); // Shared lives will be managed by controller

                        // Make player 2 observe the co-op controller
                        pc2.AddObserver(&controller);

                        // Make player 2 also observe the lives display
                        if (livesDisplayGO) {
                            auto* livesDisplay = livesDisplayGO->GetComponent<LivesDisplayComponent>();
                            if (livesDisplay) {
                                pc2.AddObserver(livesDisplay);
                            }
                        }

                        // Register player 2 with PlayerManager
                        PlayerManager::GetInstance().RegisterPlayer(player2GO.get(), 1);
                        scene.Add(player2GO);

                        // Player 2 controls (WASD + Q + E)
                        auto bindKey2 = [&](SDL_Scancode k, PlayerComponent::Direction d) {
                            input.BindCommand(k, KeyState::Down, InputDeviceType::Keyboard,
                                std::make_unique<MoveDirCommand>(&pc2, d, true), 1);
                            input.BindCommand(k, KeyState::Up, InputDeviceType::Keyboard,
                                std::make_unique<MoveDirCommand>(&pc2, d, false), 1);
                            };
                        bindKey2(SDL_SCANCODE_A, PlayerComponent::Direction::Left);
                        bindKey2(SDL_SCANCODE_D, PlayerComponent::Direction::Right);
                        bindKey2(SDL_SCANCODE_W, PlayerComponent::Direction::Up);
                        bindKey2(SDL_SCANCODE_S, PlayerComponent::Direction::Down);

                        input.BindCommand(SDL_SCANCODE_Q, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<BombCommand>(&pc2, &scene), 1);
                        input.BindCommand(SDL_SCANCODE_E, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<DetonatorCommand>(&pc2), 1);

                        // Global controls
                        input.BindCommand(SDL_SCANCODE_F2, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<dae::MuteCommand>(), -1);
                        input.BindCommand(SDL_SCANCODE_F1, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<LevelSkipCommand>(), -1);
                    }
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
                        20.f, 0.8f, 100.f,
                        walkable, glm::ivec2(cols, rows),
                        tileSize, uiOffsetY);
                    EnemyManager::GetInstance().RegisterEnemy(&oc);
                    // Also register with CoopGameController for exit spawning
                    oc.AddObserver(&controller);

                    auto& cc = enemyGO->AddComponent<CollisionComponent>();
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<EnemyCollisionResponder>(&oc));
                    oc.AddObserver(&ScoreManager::GetInstance());

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
                        18.f, 0.9f,
                        walkable, glm::ivec2(cols, rows),
                        tileSize, uiOffsetY);
                    EnemyManager::GetInstance().RegisterEnemy(&dc);
                    // Also register with CoopGameController for exit spawning
                    dc.AddObserver(&controller);

                    auto& cc = enemyGO->AddComponent<CollisionComponent>();
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<EnemyCollisionResponder>(&dc));
                    dc.AddObserver(&ScoreManager::GetInstance());

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
                    EnemyManager::GetInstance().RegisterEnemy(&bc);
                    // Also register with CoopGameController for exit spawning
                    bc.AddObserver(&controller);
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<EnemyCollisionResponder>(&bc));
                    bc.AddObserver(&ScoreManager::GetInstance());

                    scene.Add(enemyGO);
                    break;
                }
                default:
                    break;
                }
            }
        }

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);

        std::cout << "Co-op game loaded successfully." << std::endl;
    }

} // namespace dae