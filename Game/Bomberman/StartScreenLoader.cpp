#include "StartScreenLoader.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "LambdaCommand.h"
#include "ServiceLocator.h"
#include "GameModeSelector.h"
#include "DelayedCoopGameLoader.h"
#include <SDL.h>
#include "Scene.h"

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
        titleGO->AddComponent<TransformComponent>().SetLocalPosition(155.f, 20.f, 0.f);
        titleGO->AddComponent<RenderComponent>().SetTexture("BombermanTitle.tga");
        scene.Add(titleGO);

        // Game mode selection
        auto font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 16);

        // "Story Mode" option
        auto storyModeGO = std::make_shared<GameObject>();
        storyModeGO->AddComponent<TransformComponent>().SetLocalPosition(100.f, 110.f, 0.f);
        auto& storyText = storyModeGO->AddComponent<TextComponent>(font, "> Story Mode");
        scene.Add(storyModeGO);

        // "Co-op Mode" option
        auto coopModeGO = std::make_shared<GameObject>();
        coopModeGO->AddComponent<TransformComponent>().SetLocalPosition(100.f, 130.f, 0.f);
        auto& coopText = coopModeGO->AddComponent<TextComponent>(font, "  Co-op Mode");
        scene.Add(coopModeGO);

        // "Versus Mode" option  
        auto versusModeGO = std::make_shared<GameObject>();
        versusModeGO->AddComponent<TransformComponent>().SetLocalPosition(100.f, 150.f, 0.f);
        auto& versusText = versusModeGO->AddComponent<TextComponent>(font, "  Versus Mode");
        scene.Add(versusModeGO);

        // Instructions
        auto instructGO = std::make_shared<GameObject>();
        instructGO->AddComponent<TransformComponent>().SetLocalPosition(70.f, 180.f, 0.f);
        instructGO->AddComponent<TextComponent>(font, "UP/DOWN to select, SPACE to confirm");
        scene.Add(instructGO);

        // Controls info
        auto controlsTitle = std::make_shared<GameObject>();
        controlsTitle->AddComponent<TransformComponent>().SetLocalPosition(110.f, 210.f, 0.f);
        controlsTitle->AddComponent<TextComponent>(font, "CONTROLS:");
        scene.Add(controlsTitle);

        auto moveControls = std::make_shared<GameObject>();
        moveControls->AddComponent<TransformComponent>().SetLocalPosition(60.f, 230.f, 0.f);
        moveControls->AddComponent<TextComponent>(font, "P1: Arrows + X(bomb) + C(detonate)");
        scene.Add(moveControls);

        auto player2Controls = std::make_shared<GameObject>();
        player2Controls->AddComponent<TransformComponent>().SetLocalPosition(80.f, 250.f, 0.f);
        player2Controls->AddComponent<TextComponent>(font, "P2: WASD + Q(bomb) + E(detonate)");
        scene.Add(player2Controls);

        // Create game mode selector
        auto selectorGO = std::make_shared<GameObject>();
        auto& selector = selectorGO->AddComponent<GameModeSelector>();
        scene.Add(selectorGO);

        // Create the delayed co-op game loader
        auto delayedCoopLoaderGO = std::make_shared<GameObject>();
        auto& delayedCoopLoader = delayedCoopLoaderGO->AddComponent<DelayedCoopGameLoader>();
        scene.Add(delayedCoopLoaderGO);

        // Input bindings for mode selection
        InputManager::GetInstance().BindCommand(
            SDL_SCANCODE_UP,
            KeyState::Down,
            InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([&selector, &storyText, &coopText, &versusText]() {
                selector.MoveUp();
                // Update text display based on selected mode
                switch (selector.GetSelectedMode()) {
                case GameMode::Story:
                    storyText.SetText("> Story Mode");
                    coopText.SetText("  Co-op Mode");
                    versusText.SetText("  Versus Mode");
                    break;
                case GameMode::Coop:
                    storyText.SetText("  Story Mode");
                    coopText.SetText("> Co-op Mode");
                    versusText.SetText("  Versus Mode");
                    break;
                case GameMode::Versus:
                    storyText.SetText("  Story Mode");
                    coopText.SetText("  Co-op Mode");
                    versusText.SetText("> Versus Mode");
                    break;
                }
                }),
            -1
        );

        InputManager::GetInstance().BindCommand(
            SDL_SCANCODE_DOWN,
            KeyState::Down,
            InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([&selector, &storyText, &coopText, &versusText]() {
                selector.MoveDown();
                // Update text display based on selected mode
                switch (selector.GetSelectedMode()) {
                case GameMode::Story:
                    storyText.SetText("> Story Mode");
                    coopText.SetText("  Co-op Mode");
                    versusText.SetText("  Versus Mode");
                    break;
                case GameMode::Coop:
                    storyText.SetText("  Story Mode");
                    coopText.SetText("> Co-op Mode");
                    versusText.SetText("  Versus Mode");
                    break;
                case GameMode::Versus:
                    storyText.SetText("  Story Mode");
                    coopText.SetText("  Co-op Mode");
                    versusText.SetText("> Versus Mode");
                    break;
                }
                }),
            -1
        );

        InputManager::GetInstance().BindCommand(
            SDL_SCANCODE_SPACE,
            KeyState::Down,
            InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([&selector, &delayedCoopLoader]() {
                selector.TriggerSelection(&delayedCoopLoader);
                }),
            -1
        );

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);

        std::cout << "Start screen loaded successfully." << std::endl;
    }
}