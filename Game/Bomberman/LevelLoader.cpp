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

namespace dae {

    // Loads a level from the given text file.
    // Each line is one row in the level.
    // 'W' indicates a wall, 'P' indicates the player spawn.
    void LevelLoader::LoadLevel(const std::string& filename, const std::string& sceneName)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open level file: " << filename << std::endl;
            return;
        }

        // Create (or get) the scene with the provided name.
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        std::string line;
        int row = 0;
        const int tileSize = 64; // Width and height of a tile

        while (std::getline(file, line))
        {
            // For each character in the line (each column)
            for (int col = 0; col < static_cast<int>(line.size()); ++col)
            {
                char tile = line[col];
                float posX = static_cast<float>(col * tileSize);
                float posY = static_cast<float>(row * tileSize);

                if (tile == 'W')
                {
                    // Create a wall
                    auto wall = std::make_shared<GameObject>();
                    wall->AddComponent<TransformComponent>().SetLocalPosition(posX, posY, 0);
                    auto& render = wall->AddComponent<RenderComponent>();
                    render.SetTexture("wall.tga"); 
                    auto& collision = wall->AddComponent<CollisionComponent>();
                    collision.SetSize(static_cast<float>(tileSize), static_cast<float>(tileSize));
                    // block movement (static wall)
                    collision.SetResponder(std::make_unique<StaticWallResponder>());
                    scene.Add(wall);
                }
                else if (tile == 'P')
                {
                    // Create a player spawn point.
                    auto player = std::make_shared<GameObject>();
                    player->AddComponent<TransformComponent>().SetLocalPosition(posX, posY, 0);
                    auto& render = player->AddComponent<RenderComponent>();
                    render.SetTexture("player.tga"); 
                    
                    auto& collision = player->AddComponent<CollisionComponent>();
                    collision.SetSize(static_cast<float>(tileSize) * 0.8f, static_cast<float>(tileSize) * 0.8f);
                    
                    auto& playerComp = player->AddComponent<PlayerComponent>();
                    playerComp.SetHealth(3);
                    scene.Add(player);
                }
              
            }
            ++row;
        }
    }

} // namespace dae
