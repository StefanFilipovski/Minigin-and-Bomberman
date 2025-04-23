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
    // 1) Create the scene
    auto& scene = dae::SceneManager::GetInstance().CreateScene("Bomberman Player Test");

    // 2) Create the player GameObject
    auto player = std::make_shared<dae::GameObject>();
    player->AddComponent<dae::TransformComponent>()
        .SetLocalPosition(100.f, 100.f, 0.f);
    player->AddComponent<dae::RenderComponent>();

    // 3) Add and configure the sprite‐sheet (3×6 Bomberman layout)
    auto& sprite = player->AddComponent<dae::SpriteSheetComponent>();
    sprite.SetSpriteSheet("BombermanSpritesheet.tga", /*rows=*/3, /*cols=*/6,
        /*startRow=*/0, /*frameDur=*/0.15f);
    // Idle = row 0, column 4 (zero-based index)
    sprite.SetIdleFrame(dae::SpriteSheetComponent::AnimationState::Idle,
        /*rows=*/3, /*cols=*/6,
        /*row=*/0, /*idleCol=*/4);

    // 4) Add the PlayerComponent (it will grab that sprite & transform internally)
    auto& pc = player->AddComponent<dae::PlayerComponent>();

    // 5) Finalize the GameObject into the scene
    scene.Add(player);

    // 6) Wire up input → PlayerComponent
    auto& input = dae::InputManager::GetInstance();
    // Movement press
    input.BindCommand(SDL_SCANCODE_LEFT, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputPressed(SDL_SCANCODE_LEFT); }), 1);
    input.BindCommand(SDL_SCANCODE_RIGHT, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputPressed(SDL_SCANCODE_RIGHT); }), 1);
    input.BindCommand(SDL_SCANCODE_UP, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputPressed(SDL_SCANCODE_UP); }), 1);
    input.BindCommand(SDL_SCANCODE_DOWN, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputPressed(SDL_SCANCODE_DOWN); }), 1);

    // Movement release
    input.BindCommand(SDL_SCANCODE_LEFT, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputReleased(SDL_SCANCODE_LEFT); }), 1);
    input.BindCommand(SDL_SCANCODE_RIGHT, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputReleased(SDL_SCANCODE_RIGHT); }), 1);
    input.BindCommand(SDL_SCANCODE_UP, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputReleased(SDL_SCANCODE_UP); }), 1);
    input.BindCommand(SDL_SCANCODE_DOWN, KeyState::Up, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.OnInputReleased(SDL_SCANCODE_DOWN); }), 1);

    // 7) bind D to kill the player and trigger death animation
    input.BindCommand(SDL_SCANCODE_D, KeyState::Pressed, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() {
            // deal enough damage to drop to zero health
            pc.TakeDamage(pc.GetHealth());
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



