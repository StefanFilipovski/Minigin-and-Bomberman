#include "GameOverScreenLoader.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "LambdaCommand.h"
#include "StartScreenLoader.h"
#include "ScoreManager.h"
#include "ServiceLocator.h"
#include "HighScoreManager.h"
#include <SDL.h>
#include "Scene.h"
#include "DelayedStartScreenLoader.h"

namespace dae {
    void GameOverScreenLoader::LoadGameOverScreen(const std::string& sceneName)
    {
        std::cout << "Loading game over screen..." << std::endl;

        // Clear input bindings
        InputManager::GetInstance().ClearAllBindings();

        // Create the game over scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        // Stop any music
        ServiceLocator::GetSoundSystem().StopMusic();

        // Create background
        auto background = std::make_shared<GameObject>();
        background->AddComponent<TransformComponent>().SetLocalPosition(0.f, 0.f, 0.f);
        scene.Add(background);

        // Game Over title - centered
        auto titleGO = std::make_shared<GameObject>();
        titleGO->AddComponent<TransformComponent>().SetLocalPosition(95.f, 60.f, 0.f);
        auto titleFont = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 28);
        titleGO->AddComponent<TextComponent>(titleFont, "GAME OVER");
        scene.Add(titleGO);

        // Final Score display - centered
        auto scoreGO = std::make_shared<GameObject>();
        scoreGO->AddComponent<TransformComponent>().SetLocalPosition(90.f, 100.f, 0.f);
        auto font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 16);
        std::string scoreText = "Final Score: " + std::to_string(ScoreManager::GetInstance().GetScore());
        scoreGO->AddComponent<TextComponent>(font, scoreText);
        scene.Add(scoreGO);

        // High scores section
        const auto& highScores = HighScoreManager::GetInstance().GetHighScores();

        if (!highScores.empty()) {
            // High scores title 
            auto highScoreTitle = std::make_shared<GameObject>();
            highScoreTitle->AddComponent<TransformComponent>().SetLocalPosition(105.f, 140.f, 0.f);
            highScoreTitle->AddComponent<TextComponent>(font, "HIGH SCORES:");
            scene.Add(highScoreTitle);

            // Display top 3 high scores 
            for (size_t i = 0; i < std::min(size_t(3), highScores.size()); ++i) {
                auto scoreLineGO = std::make_shared<GameObject>();
                scoreLineGO->AddComponent<TransformComponent>().SetLocalPosition(85.f, 160.f + (i * 20.f), 0.f);
                std::string lineText = std::to_string(i + 1) + ". " + highScores[i].name + " - " + std::to_string(highScores[i].score);
                scoreLineGO->AddComponent<TextComponent>(font, lineText);
                scene.Add(scoreLineGO);
            }

            // Instructions to continue
            auto instructGO = std::make_shared<GameObject>();
            instructGO->AddComponent<TransformComponent>().SetLocalPosition(85.f, 240.f, 0.f);
            instructGO->AddComponent<TextComponent>(font, "Press SPACE to continue");
            scene.Add(instructGO);
        }
        else {
            // No high scores yet
            auto noScoresGO = std::make_shared<GameObject>();
            noScoresGO->AddComponent<TransformComponent>().SetLocalPosition(70.f, 140.f, 0.f);
            noScoresGO->AddComponent<TextComponent>(font, "Be the first to set a high score!");
            scene.Add(noScoresGO);

            // Instructions to continue
            auto instructGO = std::make_shared<GameObject>();
            instructGO->AddComponent<TransformComponent>().SetLocalPosition(85.f, 180.f, 0.f);
            instructGO->AddComponent<TextComponent>(font, "Press SPACE to continue");
            scene.Add(instructGO);
        }

        // Create the delayed start screen loader
        auto delayedLoaderGO = std::make_shared<GameObject>();
        auto& delayedLoader = delayedLoaderGO->AddComponent<DelayedStartScreenLoader>();
        scene.Add(delayedLoaderGO);

        // Input binding that uses the delayed loader
        InputManager::GetInstance().BindCommand(
            SDL_SCANCODE_SPACE,
            KeyState::Down,
            InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([&delayedLoader]() {
                std::cout << "Space pressed - returning to start screen..." << std::endl;
                delayedLoader.TriggerStartScreenLoad();
                }),
            -1
        );

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);

        std::cout << "Game over screen loaded successfully." << std::endl;
    }
}

