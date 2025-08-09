#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm.hpp>

namespace dae {

    // Forward declarations
    class Scene;
    class GameObject;

    class LevelLoader {
    public:
        void LoadLevel(const std::string& filename, const std::string& sceneName);

    private:
        // Constants
        static constexpr int TILE_SIZE = 20;
        static constexpr int UI_HEIGHT = 40;

        // Level data
        std::vector<std::vector<char>> m_levelGrid;
        std::vector<std::vector<bool>> m_platforms;  // For movement logic
        std::vector<std::vector<bool>> m_ladders;    // For movement logic
        int m_gridWidth = 0;
        int m_gridHeight = 0;

        // Track created entities for input binding
        std::map<int, GameObject*> m_players;  // PlayerID -> GameObject pointer

        // Core functions
        glm::vec2 GridToWorld(int gridX, int gridY) const;
        void ParseLevelFile(const std::string& filename);

        // Creation functions
        void CreateCollisionBoxes(Scene& scene);
        void CreateEntities(Scene& scene);
        void SetupInput();

        // Specific creation helpers
        void CreatePlatformCollision(Scene& scene, int gridX, int gridY);
        void CreateLadderCollision(Scene& scene, int gridX, int gridY);
        void CreatePlayer(Scene& scene, int gridX, int gridY, int playerID);

        // Future entity creation (placeholders for now)
        // void CreateEnemy(Scene& scene, int gridX, int gridY, char enemyType);
        // void CreateIngredient(Scene& scene, int gridX, int gridY, char ingredientType, int burgerID);
        // void CreatePlate(Scene& scene, int gridX, int gridY);
    };
}
