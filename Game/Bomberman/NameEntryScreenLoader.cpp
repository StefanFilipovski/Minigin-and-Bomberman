#include "NameEntryScreenLoader.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "LambdaCommand.h"
#include "ServiceLocator.h"
#include "DelayedLevelLoader.h"
#include "PlayerNameManager.h"
#include <SDL.h>
#include "Scene.h"

namespace dae {
    void NameEntryScreenLoader::LoadNameEntryScreen(const std::string& sceneName)
    {
        std::cout << "Loading name entry screen..." << std::endl;

        // Clear input bindings
        InputManager::GetInstance().ClearAllBindings();

        // Create the name entry scene
        auto& scene = SceneManager::GetInstance().CreateScene(sceneName);

        // Keep title screen music playing
        // ServiceLocator::GetSoundSystem() - don't stop music here

        // Create background
        auto background = std::make_shared<GameObject>();
        background->AddComponent<TransformComponent>().SetLocalPosition(0.f, 0.f, 0.f);
        scene.Add(background);

        // Title
        auto titleGO = std::make_shared<GameObject>();
        titleGO->AddComponent<TransformComponent>().SetLocalPosition(80.f, 60.f, 0.f);
        auto titleFont = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 24);
        titleGO->AddComponent<TextComponent>(titleFont, "ENTER YOUR NAME");
        scene.Add(titleGO);

        // Current name display
        auto nameDisplayGO = std::make_shared<GameObject>();
        nameDisplayGO->AddComponent<TransformComponent>().SetLocalPosition(120.f, 120.f, 0.f);
        auto font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 20);
        auto& nameDisplay = nameDisplayGO->AddComponent<TextComponent>(font, "___");
        scene.Add(nameDisplayGO);

        // Instructions
        auto instructGO = std::make_shared<GameObject>();
        instructGO->AddComponent<TransformComponent>().SetLocalPosition(70.f, 160.f, 0.f);
        auto smallFont = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 14);
        instructGO->AddComponent<TextComponent>(smallFont, "Type 3 letters (A-Z)");
        scene.Add(instructGO);

        auto enterInstructGO = std::make_shared<GameObject>();
        enterInstructGO->AddComponent<TransformComponent>().SetLocalPosition(80.f, 180.f, 0.f);
        enterInstructGO->AddComponent<TextComponent>(smallFont, "Press ENTER when done");
        scene.Add(enterInstructGO);

        auto backspaceInstructGO = std::make_shared<GameObject>();
        backspaceInstructGO->AddComponent<TransformComponent>().SetLocalPosition(75.f, 200.f, 0.f);
        backspaceInstructGO->AddComponent<TextComponent>(smallFont, "BACKSPACE to delete");
        scene.Add(backspaceInstructGO);

        // Create the delayed level loader
        auto delayedLoaderGO = std::make_shared<GameObject>();
        auto& delayedLoader = delayedLoaderGO->AddComponent<DelayedLevelLoader>();
        scene.Add(delayedLoaderGO);

        // Set up input bindings for all letters A-Z
        for (int i = 0; i < 26; ++i) {
            SDL_Scancode key = static_cast<SDL_Scancode>(SDL_SCANCODE_A + i);
            char letter = static_cast<char>('A' + i);

            InputManager::GetInstance().BindCommand(
                key,
                KeyState::Down,
                InputDeviceType::Keyboard,
                std::make_unique<LambdaCommand>([letter, &nameDisplay]() {
                    PlayerNameManager::GetInstance().AddLetter(letter);
                    nameDisplay.SetText(PlayerNameManager::GetInstance().GetDisplayName());
                    }),
                -1
            );
        }

        // Backspace to remove letters
        InputManager::GetInstance().BindCommand(
            SDL_SCANCODE_BACKSPACE,
            KeyState::Down,
            InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([&nameDisplay]() {
                PlayerNameManager::GetInstance().RemoveLetter();
                nameDisplay.SetText(PlayerNameManager::GetInstance().GetDisplayName());
                }),
            -1
        );

        // Enter to confirm and start game
        InputManager::GetInstance().BindCommand(
            SDL_SCANCODE_RETURN,
            KeyState::Down,
            InputDeviceType::Keyboard,
            std::make_unique<LambdaCommand>([&delayedLoader]() {
                if (PlayerNameManager::GetInstance().IsNameComplete()) {
                    std::cout << "Name entered: " << PlayerNameManager::GetInstance().GetPlayerName() << std::endl;
                    delayedLoader.TriggerLevelLoad();
                }
                }),
            -1
        );

        // Set as active scene
        SceneManager::GetInstance().SetActiveScene(sceneName);

        std::cout << "Name entry screen loaded successfully." << std::endl;
    }
}