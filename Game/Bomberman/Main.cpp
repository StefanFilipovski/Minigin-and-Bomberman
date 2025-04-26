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
#include "ServiceLocator.h"
#include "SDLMixerSoundSystem.h"


struct HitLogger : public dae::Observer {
    void OnNotify(const dae::Event& ev) override {
        if (ev.id == GameEvents::PLAYER_HIT)
            std::cout << "[Logger] Player was hit!\n";
        else if (ev.id == GameEvents::PLAYER_DIED)
            std::cout << "[Logger] Player has died.\n";
    }
};

void load()
{

    auto& scene = dae::SceneManager::GetInstance().CreateScene("Controller + Keyboard Demo");


    auto player = std::make_shared<dae::GameObject>();
    player->AddComponent<dae::TransformComponent>().SetLocalPosition(100.f, 100.f, 0.f);
    player->AddComponent<dae::RenderComponent>();


    auto& sprite = player->AddComponent<dae::SpriteSheetComponent>();
    sprite.SetSpriteSheet("BombermanSpritesheet.tga", 3, 6, 0, 0.15f);
    sprite.SetIdleFrame(
        dae::SpriteSheetComponent::AnimationState::Idle,
        3, 6, 0, 4
    );


    auto& pc = player->AddComponent<dae::PlayerComponent>();
    auto& lives = player->AddComponent<dae::LivesDisplay>(3);
    pc.AddObserver(&lives);

    scene.Add(player);


    auto& input = dae::InputManager::GetInstance();


    auto bindDU = [&](int code, InputDeviceType dev, int pIdx,
        dae::PlayerComponent::Direction dir)
        {
        
            input.BindCommand(code, KeyState::Down, dev,
                std::make_unique<dae::LambdaCommand>(
                    [&pc, dir]() { pc.OnMovementPressed(dir); }
                ), pIdx);
            
            input.BindCommand(code, KeyState::Up, dev,
                std::make_unique<dae::LambdaCommand>(
                    [&pc, dir]() { pc.OnMovementReleased(dir); }
                ), pIdx);
        };


    bindDU(SDL_SCANCODE_LEFT, InputDeviceType::Keyboard, 1, dae::PlayerComponent::Direction::Left);
    bindDU(SDL_SCANCODE_RIGHT, InputDeviceType::Keyboard, 1, dae::PlayerComponent::Direction::Right);
    bindDU(SDL_SCANCODE_UP, InputDeviceType::Keyboard, 1, dae::PlayerComponent::Direction::Up);
    bindDU(SDL_SCANCODE_DOWN, InputDeviceType::Keyboard, 1, dae::PlayerComponent::Direction::Down);

  
    bindDU(int(dae::GamepadButton::DPadLeft), InputDeviceType::Gamepad, 0, dae::PlayerComponent::Direction::Left);
    bindDU(int(dae::GamepadButton::DPadRight), InputDeviceType::Gamepad, 0, dae::PlayerComponent::Direction::Right);
    bindDU(int(dae::GamepadButton::DPadUp), InputDeviceType::Gamepad, 0, dae::PlayerComponent::Direction::Up);
    bindDU(int(dae::GamepadButton::DPadDown), InputDeviceType::Gamepad, 0, dae::PlayerComponent::Direction::Down);

    //test damage keys
    input.BindCommand(SDL_SCANCODE_H, KeyState::Down, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.TakeDamage(1); }), 1);
    input.BindCommand(SDL_SCANCODE_D, KeyState::Down, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([&pc]() { pc.TakeDamage(pc.GetHealth()); }), 1);
}



int main(int, char* [])
{
    try {
       
        dae::Minigin engine("../Data/");
        ServiceLocator::RegisterSoundSystem(
            std::make_unique<SDLMixerSoundSystem>("../Data/"));
        engine.Run(load);
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
    dae::DebugUIManager::Shutdown();
    return 0;
}



