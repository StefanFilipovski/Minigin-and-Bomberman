#include "VersusGameLoader.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "ServiceLocator.h"
#include "PlayerComponent.h"
#include "Player2BalloonComponent.h"
#include "SpriteSheetComponent.h"
#include "CollisionComponent.h"
#include "StaticWallResponder.h"
#include "Player2CollisionResponder.h"
#include "MoveDirCommand.h"
#include "BombCommand.h"
#include "DetonatorCommand.h"
#include "VersusGameController.h"
#include <fstream>
#include "LambdaCommand.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "CollisionManager.h"
#include "Player2DetonatorCommand.h"
#include "Player2BombCommand.h"
#include "Player2MoveDirCommand.h"

namespace dae {
    void VersusGameLoader::LoadVersusGame(const std::string& sceneName)
    {
        std::cout << "Loading versus game..." << std::endl;

        // IMPORTANT: Clear everything properly to avoid lag
        EnemyManager::GetInstance().ClearLevel();
        PlayerManager::GetInstance().ClearPlayers();
        CollisionManager::GetInstance().Clear();

        // Clear input bindings
        InputManager::GetInstance().ClearAllBindings();

        // Create the versus scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        // Play versus mode music
        ServiceLocator::GetSoundSystem().StopMusic();
        ServiceLocator::GetSoundSystem().PlayMusic("Bomberman (NES) Music - Stage Theme.ogg", 0.7f);

        // Constants
        const float tileSize = 16.f;
        constexpr int uiRows = 4;
        const float uiOffsetY = tileSize * uiRows;

        // Load versus map (we'll use level1.txt for simplicity)
        std::ifstream file("../Data/versuslevel.txt");
        std::vector<std::string> mapRows;
        std::string line;
        while (std::getline(file, line)) {
            mapRows.push_back(line);
        }
        file.close();

        int rows = static_cast<int>(mapRows.size());
        int cols = rows > 0 ? static_cast<int>(mapRows[0].size()) : 0;

        // Create background
        auto metalBg = std::make_shared<GameObject>();
        metalBg->AddComponent<TransformComponent>().SetLocalPosition(0.f, 0.f, 0.f);
        metalBg->AddComponent<RenderComponent>().SetTexture("MetalBackground.tga");
        scene.Add(metalBg);

        auto grassBg = std::make_shared<GameObject>();
        grassBg->AddComponent<TransformComponent>().SetLocalPosition(0.f, uiOffsetY, 0.f);
        grassBg->AddComponent<RenderComponent>().SetTexture("GrassBackground.tga");
        scene.Add(grassBg);

        // Create versus game controller
        auto gameController = std::make_shared<GameObject>();
        auto& controller = gameController->AddComponent<VersusGameController>();
        scene.Add(gameController);

        // Spawn map objects and players
        GameObject* player1GO = nullptr;
        GameObject* player2GO = nullptr;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                char tile = mapRows[r][c];
                float x = c * tileSize;
                float y = r * tileSize + uiOffsetY;

                switch (tile) {
                case 'W': {
                    // Static walls
                    auto wall = std::make_shared<GameObject>();
                    wall->AddComponent<TransformComponent>().SetLocalPosition(x, y, 0.f);
                    wall->AddComponent<RenderComponent>().SetTexture("StaticWall.tga");
                    auto& cc = wall->AddComponent<CollisionComponent>();
                    cc.SetSize(tileSize, tileSize);
                    cc.SetResponder(std::make_unique<StaticWallResponder>());
                    scene.Add(wall);
                    break;
                }
                case 'P': {
                    // Player 1 (Bomberman)
                    constexpr float pxOff = 7.5f, pyOff = 5.f;
                    auto playerGO = std::make_shared<GameObject>();
                    playerGO->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + pxOff, y + pyOff, 0.f);

                    auto& spr = playerGO->AddComponent<SpriteSheetComponent>();
                    spr.SetSpriteSheet("BombermanSpritesheet.tga", 3, 6, 0, 0.15f);
                    spr.SetIdleFrame(SpriteSheetComponent::AnimationState::Idle, 3, 6, 0, 4);

                    auto& pcc = playerGO->AddComponent<CollisionComponent>();
                    float cSize = tileSize * 0.8f;
                    pcc.SetSize(cSize, cSize + 2);
                    float off = (tileSize - cSize) * 0.5f;
                    pcc.SetOffset(off - pxOff, off - pyOff);

                    auto& pc = playerGO->AddComponent<PlayerComponent>();
                    pc.BeginMove();
                    pc.SetLives(3);

                    // Make game controller observe player 1
                    pc.AddObserver(&controller);

                    scene.Add(playerGO);
                    player1GO = playerGO.get();

                    // Player 1 controls (Arrow keys)
                    auto& input = InputManager::GetInstance();
                    auto bindKey = [&](SDL_Scancode k, PlayerComponent::Direction d) {
                        input.BindCommand(k, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<MoveDirCommand>(&pc, d, true), 0);
                        input.BindCommand(k, KeyState::Up, InputDeviceType::Keyboard,
                            std::make_unique<MoveDirCommand>(&pc, d, false), 0);
                        };
                    bindKey(SDL_SCANCODE_LEFT, PlayerComponent::Direction::Left);
                    bindKey(SDL_SCANCODE_RIGHT, PlayerComponent::Direction::Right);
                    bindKey(SDL_SCANCODE_UP, PlayerComponent::Direction::Up);
                    bindKey(SDL_SCANCODE_DOWN, PlayerComponent::Direction::Down);
                    input.BindCommand(SDL_SCANCODE_X, KeyState::Down, InputDeviceType::Keyboard,
                        std::make_unique<BombCommand>(&pc, &scene), 0);
                    input.BindCommand(SDL_SCANCODE_C, KeyState::Down, InputDeviceType::Keyboard,
                        std::make_unique<DetonatorCommand>(&pc), 0);
                    break;
                }
                case 'E': {
                    // Player 2 (Balloon - controlled by second player)
                    constexpr float spriteOffset = 8.f;
                    auto player2GameObject = std::make_shared<GameObject>();
                    player2GameObject->AddComponent<TransformComponent>()
                        .SetLocalPosition(x + spriteOffset, y + spriteOffset, 0.f);

                    player2GameObject->AddComponent<SpriteSheetComponent>()
                        .SetSpriteSheet("BalloomSpritesheet.tga", 1, 11, 0, 0.2f);

                    auto& p2c = player2GameObject->AddComponent<Player2BalloonComponent>();
                    p2c.SetLives(3);

                    // Make game controller observe player 2
                    p2c.AddObserver(&controller);

                    auto& cc = player2GameObject->AddComponent<CollisionComponent>();
                    float collSize = tileSize * 0.8f;
                    float baseOff = (tileSize - collSize) * 0.5f;
                    cc.SetSize(collSize, collSize);
                    cc.SetOffset(-spriteOffset + baseOff, -spriteOffset + baseOff);
                    cc.SetResponder(std::make_unique<Player2CollisionResponder>(&p2c));

                    scene.Add(player2GameObject);
                    player2GO = player2GameObject.get();

                    // Player 2 controls (WASD)
                    auto& input = InputManager::GetInstance();
                    auto bindKey2 = [&](SDL_Scancode k, Player2BalloonComponent::Direction d) {
                        input.BindCommand(k, KeyState::Down, InputDeviceType::Keyboard,
                            std::make_unique<Player2MoveDirCommand>(&p2c, d, true), 1);
                        input.BindCommand(k, KeyState::Up, InputDeviceType::Keyboard,
                            std::make_unique<Player2MoveDirCommand>(&p2c, d, false), 1);
                        };
                    bindKey2(SDL_SCANCODE_A, Player2BalloonComponent::Direction::Left);
                    bindKey2(SDL_SCANCODE_D, Player2BalloonComponent::Direction::Right);
                    bindKey2(SDL_SCANCODE_W, Player2BalloonComponent::Direction::Up);
                    bindKey2(SDL_SCANCODE_S, Player2BalloonComponent::Direction::Down);
                    input.BindCommand(SDL_SCANCODE_Q, KeyState::Down, InputDeviceType::Keyboard,
                        std::make_unique<Player2BombCommand>(&p2c, &scene), 1);
                    input.BindCommand(SDL_SCANCODE_E, KeyState::Down, InputDeviceType::Keyboard,
                        std::make_unique<Player2DetonatorCommand>(&p2c), 1);
                    break;
                }

                default:
                    break;
                }
            }
        }

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);

        std::cout << "Versus game loaded successfully." << std::endl;
    }
}