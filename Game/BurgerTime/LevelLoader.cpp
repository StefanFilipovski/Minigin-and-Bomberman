#include "LevelLoader.h"

#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"

#include "PlatformRenderer.h"
#include "SpriteSheetComponent.h"
#include "CollisionComponent.h"
#include "PlayerMovementComponent.h"
#include "InputManager.h"
#include "Camera.h"
#include "MoveCommand.h"

#include "PlatformCollisionResponder.h"
#include "LadderCollisionResponder.h"

#include <SDL.h>
#include <fstream>
#include <iostream>

namespace dae {

    // Grid -> world (top-left origin) with UI top margin
    glm::vec2 LevelLoader::GridToWorld(int gridX, int gridY) const {
        return glm::vec2(
            gridX * TILE_SIZE,
            gridY * TILE_SIZE + UI_HEIGHT
        );
    }

    void LevelLoader::LoadLevel(const std::string& filename, const std::string& sceneName) {
        std::cout << "Loading level: " << filename << std::endl;

        // Parse the level file
        ParseLevelFile(filename);

        // Preload textures
        auto& rm = ResourceManager::GetInstance();
        rm.LoadTexture("ChefSprite.tga");

        // Create scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        // Background holder (if you add any background later)
        auto background = std::make_shared<GameObject>();
        background->AddComponent<TransformComponent>().SetLocalPosition(0, 0, 0);
        scene.Add(background);

        // Visual renderer for platforms/ladders
        auto levelRenderer = std::make_shared<GameObject>();
        levelRenderer->AddComponent<TransformComponent>().SetLocalPosition(0, 0, 0);
        levelRenderer->AddComponent<PlatformRenderer>(
            m_levelGrid, m_gridWidth, m_gridHeight, TILE_SIZE, UI_HEIGHT);
        scene.Add(levelRenderer);

        // Collision boxes for platforms/ladders (used by your movement/ingredients later)
        CreateCollisionBoxes(scene);

        // Entities (players, enemies later, etc.)
        CreateEntities(scene);

        // Input
        SetupInput();

        // Activate
        SceneManager::GetInstance().SetActiveScene(sceneName);

        std::cout << "Level loaded successfully!" << std::endl;
    }

    void LevelLoader::ParseLevelFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open level file: " << filename << std::endl;
            return;
        }

        // Read dimensions
        file >> m_gridWidth >> m_gridHeight;
        file.ignore();  // skip newline

        // Init grids
        m_levelGrid.resize(m_gridHeight, std::vector<char>(m_gridWidth, ' '));
        m_platforms.resize(m_gridHeight, std::vector<bool>(m_gridWidth, false));
        m_ladders.resize(m_gridHeight, std::vector<bool>(m_gridWidth, false));

        // Read rows
        std::string line;
        for (int y = 0; y < m_gridHeight && std::getline(file, line); ++y) {
            for (int x = 0; x < m_gridWidth && x < static_cast<int>(line.size()); ++x) {
                char tile = line[x];
                m_levelGrid[y][x] = tile;

                // Mark platforms for movement
                if (tile == '=' || tile == '+' || tile == '#') {
                    m_platforms[y][x] = true;
                }
                // Mark ladders for movement
                if (tile == 'L' || tile == 'H' || tile == '|' || tile == '#') {
                    m_ladders[y][x] = true;
                }
            }
        }

        file.close();
        std::cout << "Parsed level: " << m_gridWidth << "x" << m_gridHeight << std::endl;
    }

    void LevelLoader::CreateCollisionBoxes(Scene& scene) {
        for (int y = 0; y < m_gridHeight; ++y) {
            for (int x = 0; x < m_gridWidth; ++x) {
                const char tile = m_levelGrid[y][x];

                // Platforms
                if (tile == '=' || tile == '+' || tile == '#') {
                    CreatePlatformCollision(scene, x, y);
                }
                // Ladders
                if (tile == 'L' || tile == 'H' || tile == '|' || tile == '#') {
                    CreateLadderCollision(scene, x, y);
                }
            }
        }
    }

    void LevelLoader::CreatePlatformCollision(Scene& scene, int gridX, int gridY) {
        auto platform = std::make_shared<GameObject>();
        const auto worldPos = GridToWorld(gridX, gridY);
        platform->AddComponent<TransformComponent>().SetLocalPosition(worldPos.x, worldPos.y, 0);

        // Collision: thin strip on top of tile
        auto& collision = platform->AddComponent<CollisionComponent>();
        collision.SetSize(static_cast<float>(TILE_SIZE), 4.0f);
        collision.SetOffset(0.0f, 0.0f);

        // Attach your (no-op) platform responder to keep the design consistent
        collision.SetResponder(std::make_unique<PlatformCollisionResponder>());  // uses existing API

        scene.Add(platform);
    }

    void LevelLoader::CreateLadderCollision(Scene& scene, int gridX, int gridY) {
        auto ladder = std::make_shared<GameObject>();
        const auto worldPos = GridToWorld(gridX, gridY);
        ladder->AddComponent<TransformComponent>().SetLocalPosition(worldPos.x, worldPos.y, 0);

        // Collision: centered ladder band
        auto& collision = ladder->AddComponent<CollisionComponent>();
        collision.SetSize(TILE_SIZE * 0.6f, static_cast<float>(TILE_SIZE));
        collision.SetOffset(TILE_SIZE * 0.2f, 0.0f);

        // Attach your (no-op) ladder responder
        collision.SetResponder(std::make_unique<LadderCollisionResponder>());    // uses existing API

        scene.Add(ladder);
    }

    void LevelLoader::CreateEntities(Scene& scene) {
        for (int y = 0; y < m_gridHeight; ++y) {
            for (int x = 0; x < m_gridWidth; ++x) {
                const char tile = m_levelGrid[y][x];

                switch (tile) {
                case 'P': // Player 1
                    CreatePlayer(scene, x, y, 0);
                    break;
                case 'S': // Player 2 (future co-op)
                    // CreatePlayer(scene, x, y, 1);
                    break;
                default:
                    break;
                }
            }
        }
    }

    void LevelLoader::CreatePlayer(Scene& scene, int gridX, int gridY, int playerID) {
        auto player = std::make_shared<GameObject>();

        const auto worldPos = GridToWorld(gridX, gridY);
        player->AddComponent<TransformComponent>().SetLocalPosition(worldPos.x, worldPos.y, 0);

        // Visuals
        auto& sprite = player->AddComponent<SpriteSheetComponent>();
        sprite.SetSpriteSheet("ChefSprite.tga", 2, 9, 0, 0.15f);
        sprite.SetIdleFrame(SpriteSheetComponent::AnimationState::Idle, 2, 9, 0, 3);
        sprite.SetScale(1.5f, 1.5f);

        // Player collision (no layer/filter API; player logic queries collisions)
        auto& collision = player->AddComponent<CollisionComponent>();
        collision.SetSize(TILE_SIZE * 0.7f, TILE_SIZE * 0.9f);
        collision.SetOffset(TILE_SIZE * 0.15f, TILE_SIZE * 0.05f);
        // No SetLayer / SetCollidesWith here (not in API). Responder optional.

        // Movement needs platform/ladder boolean grids + tile info
        auto& movement = player->AddComponent<PlayerMovementComponent>(
            m_platforms, m_ladders, TILE_SIZE, gridX, gridY
        );
        (void)movement;

        // Track for input binding
        m_players[playerID] = player.get();

        // Camera follows P1
        if (playerID == 0) {
            Camera::GetInstance().SetTarget(player);
        }

        scene.Add(player);

        std::cout << "Created player " << playerID
            << " at grid (" << gridX << ", " << gridY << ")\n";
    }

    void LevelLoader::SetupInput()
    {
        auto& input = InputManager::GetInstance();

        // (Optional) Remove this if your engine already has global bindings you need
        input.ClearAllBindings();

        // Bind controls for Player 1 if present
        auto it = m_players.find(0);
        if (it == m_players.end() || !it->second) {
            std::cout << "[Input] No Player 0 found to bind.\n";
            return;
        }

        auto* player = it->second;
        auto* movement = player->GetComponent<PlayerMovementComponent>();
        if (!movement) {
            std::cout << "[Input] Player 0 has no PlayerMovementComponent.\n";
            return;
        }

        // ALSO bind continuous "Pressed" so holding keys moves continuously
        input.BindCommand(SDL_SCANCODE_LEFT, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Left), 0);
        input.BindCommand(SDL_SCANCODE_RIGHT, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Right), 0);
        input.BindCommand(SDL_SCANCODE_UP, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Up), 0);
        input.BindCommand(SDL_SCANCODE_DOWN, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Down), 0);

        // WASD aliases (Pressed)
        input.BindCommand(SDL_SCANCODE_A, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Left), 0);
        input.BindCommand(SDL_SCANCODE_D, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Right), 0);
        input.BindCommand(SDL_SCANCODE_W, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Up), 0);
        input.BindCommand(SDL_SCANCODE_S, KeyState::Pressed, InputDeviceType::Keyboard,
            std::make_unique<MoveCommand>(movement, Direction::Down), 0);


        // Stop movement on key up (release = idle)
        for (auto sc : { SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
                         SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S })
        {
            input.BindCommand(sc, KeyState::Up, InputDeviceType::Keyboard,
                std::make_unique<MoveCommand>(movement, Direction::None), 0);
        }

        std::cout << "[Input] Keyboard movement bound for Player 0 (arrows + WASD).\n";
    }


} // namespace dae
