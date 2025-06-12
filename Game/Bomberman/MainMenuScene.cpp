#include "MainMenuScene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "LambdaCommand.h"
#include "LevelManager.h"
#include "GameState.h"
#include <SDL.h>
#include "Scene.h"

namespace dae {
    void MainMenuScene::CreateMainMenu(const std::string& sceneName)
    {
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        // Black background
        auto background = std::make_shared<GameObject>();
        background->AddComponent<TransformComponent>().SetLocalPosition(0.f, 0.f, 0.f);
        auto& rc = background->AddComponent<RenderComponent>();
        rc.SetTexture("MetalBackground.tga");
        scene.Add(background);

        // Title text
        auto titleGO = std::make_shared<GameObject>();
        titleGO->AddComponent<TransformComponent>().SetLocalPosition(200.f, 100.f, 0.f);
        auto font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 24);
        titleGO->AddComponent<TextComponent>(font, "BOMBERMAN");
        scene.Add(titleGO);

        // Press Enter text
        auto instructionGO = std::make_shared<GameObject>();
        instructionGO->AddComponent<TransformComponent>().SetLocalPosition(220.f, 200.f, 0.f);
        auto smallFont = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 16);
        instructionGO->AddComponent<TextComponent>(smallFont, "Press ENTER to Start");
        scene.Add(instructionGO);

        // Bind Enter key to start game
        auto& input = InputManager::GetInstance();
        input.ClearAllBindings();

        input.BindCommand(SDL_SCANCODE_RETURN, KeyState::Down, InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([]() {
                // Change state and load level
                GameStateManager::SetState(GameState::Playing);
                LevelManager::GetInstance().LoadLevel(0);
                SceneManager::GetInstance().SetActiveScene("Level 1");
            }), -1);
    }
}