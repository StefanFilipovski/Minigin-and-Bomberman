#include "StartScreenLoader.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "LambdaCommand.h"
#include "LevelManager.h"
#include "ScoreManager.h"
#include "ServiceLocator.h"
#include <SDL.h>
#include "Scene.h"
#include "DelayedLevelLoader.h"
#include <RenderComponent.h>
#include "DelayedNameEntryLoader.h"

namespace dae {
    void StartScreenLoader::LoadStartScreen(const std::string& sceneName)
    {
        std::cout << "Loading start screen..." << std::endl;

        // Clear input bindings
        InputManager::GetInstance().ClearAllBindings();

        // Create the start screen scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        // Stop any existing music and play title screen music
        ServiceLocator::GetSoundSystem().StopMusic();
        ServiceLocator::GetSoundSystem().PlayMusic("Bomberman (NES) Music - Title Screen.ogg", 0.6f);

        // Create background
        auto background = std::make_shared<GameObject>();
        background->AddComponent<TransformComponent>().SetLocalPosition(0.f, 0.f, 0.f);
        scene.Add(background);

        // Title image
        auto titleGO = std::make_shared<GameObject>();
        titleGO->AddComponent<TransformComponent>().SetLocalPosition(155.f, 0.f, 0.f);
        titleGO->AddComponent<RenderComponent>().SetTexture("BombermanTitle.tga");
        scene.Add(titleGO);

        // "Press SPACE to start" text - centered
        auto startTextGO = std::make_shared<GameObject>();
        startTextGO->AddComponent<TransformComponent>().SetLocalPosition(150.f, 150.f, 0.f);
        auto font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 16);
        startTextGO->AddComponent<TextComponent>(font, "Press SPACE to start");
        scene.Add(startTextGO);

        // Controls info - centered and spaced nicely
        auto controlsTitle = std::make_shared<GameObject>();
        controlsTitle->AddComponent<TransformComponent>().SetLocalPosition(110.f, 190.f, 0.f);
        controlsTitle->AddComponent<TextComponent>(font, "CONTROLS:");
        scene.Add(controlsTitle);

        auto moveControls = std::make_shared<GameObject>();
        moveControls->AddComponent<TransformComponent>().SetLocalPosition(80.f, 210.f, 0.f);
        moveControls->AddComponent<TextComponent>(font, "Arrow keys - Move");
        scene.Add(moveControls);

        auto bombControls = std::make_shared<GameObject>();
        bombControls->AddComponent<TransformComponent>().SetLocalPosition(80.f, 230.f, 0.f);
        bombControls->AddComponent<TextComponent>(font, "X - Place bomb");
        scene.Add(bombControls);

        auto detonatorControls = std::make_shared<GameObject>();
        detonatorControls->AddComponent<TransformComponent>().SetLocalPosition(80.f, 250.f, 0.f);
        detonatorControls->AddComponent<TextComponent>(font, "C - Detonate bomb");
        scene.Add(detonatorControls);

        // Create the delayed name entry loader
        auto delayedLoaderGO = std::make_shared<GameObject>();
        auto& delayedLoader = delayedLoaderGO->AddComponent<DelayedNameEntryLoader>();
        scene.Add(delayedLoaderGO);

        // Input binding that goes to name entry screen
        InputManager::GetInstance().BindCommand(
            SDL_SCANCODE_SPACE,
            KeyState::Down,
            InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([&delayedLoader]() {
                std::cout << "Space pressed - going to name entry..." << std::endl;
                delayedLoader.TriggerNameEntryLoad();
                }),
            -1
        );

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);

        std::cout << "Start screen loaded successfully." << std::endl;
    }
}

