#include <SDL.h>

//#if _DEBUG
//#if !defined(VLD_CUSTOM_CONFIG)
//#define VLD_CUSTOM_CONFIG
//#endif
//#if __has_include(<vld.h>)
//#include <vld.h>
//#endif
//#endif



#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "Scene.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "FPSCounter.h"
#include "CharacterMoveComponent.h"
#include "UpDownMoveComponent.h"
#include "GameActor.h"
#include "InputManager.h"
#include "GameActorCommand.cpp"
#include "MoveCommand.h"
#include "LivesDisplay.h"
#include "DeathCommand.h"
#include "PointsDisplay.h"
#include "PointsSubject.h"
#include "AddPointsCommand.h"
#include <DebugUIManager.h>
#include "SpriteSheetComponent.h"
#include "CollisionManager.h"  
#include "CollisionComponent.h" 
#include "LambdaCommand.h"
#include "StaticWallResponder.h"
#include "CollisionResponder.h" 
#include "PlayerComponent.h"
#include "LevelLoader.h"
#include "Camera.h"


void load()
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene("Test Sprite Animation");

    // Create the player object.
    auto player = std::make_shared<dae::GameObject>();
    player->AddComponent<dae::TransformComponent>().SetLocalPosition(100, 100, 0);
    player->AddComponent<dae::RenderComponent>();

    // Attach the SpriteSheetComponent.
    auto& spriteComp = player->AddComponent<dae::SpriteSheetComponent>();

    // Load the new sprite sheet.
    // "TestSpriteSheet.tga" is organized in 4 rows and 3 columns.
    spriteComp.SetSpriteSheet("TestSpriteSheet.tga", 4, 3, 0, 0.15f);

    // Set the initial idle state (using Down facing: row 2, column 1).
    spriteComp.SetIdleFrame(dae::SpriteSheetComponent::AnimationState::Idle, 4, 3, 2, 1);

    scene.Add(player);

    auto& inputManager = dae::InputManager::GetInstance();

    // Bind movement key press events.
    inputManager.BindCommand(SDL_SCANCODE_UP, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyPressed(SDL_SCANCODE_UP);
            }), 1);
    inputManager.BindCommand(SDL_SCANCODE_RIGHT, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyPressed(SDL_SCANCODE_RIGHT);
            }), 1);
    inputManager.BindCommand(SDL_SCANCODE_DOWN, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyPressed(SDL_SCANCODE_DOWN);
            }), 1);
    inputManager.BindCommand(SDL_SCANCODE_LEFT, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyPressed(SDL_SCANCODE_LEFT);
            }), 1);

    // Bind movement key release events.
    inputManager.BindCommand(SDL_SCANCODE_UP, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyReleased(SDL_SCANCODE_UP);
            }), 1);
    inputManager.BindCommand(SDL_SCANCODE_RIGHT, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyReleased(SDL_SCANCODE_RIGHT);
            }), 1);
    inputManager.BindCommand(SDL_SCANCODE_DOWN, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyReleased(SDL_SCANCODE_DOWN);
            }), 1);
    inputManager.BindCommand(SDL_SCANCODE_LEFT, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&spriteComp]() {
            spriteComp.OnMovementKeyReleased(SDL_SCANCODE_LEFT);
            }), 1);
}

int main(int, char* [])
{
    try {
       
        dae::Minigin engine("../Data/");
        engine.Run(load);
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
    dae::DebugUIManager::Shutdown();
    return 0;
}



