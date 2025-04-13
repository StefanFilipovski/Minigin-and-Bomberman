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
  
    auto& scene = dae::SceneManager::GetInstance().CreateScene("Camera Test");

   
    auto background = std::make_shared<dae::GameObject>();
    background->AddComponent<dae::TransformComponent>().SetLocalPosition(0, 0, 0);
    auto& bgRender = background->AddComponent<dae::RenderComponent>();
    bgRender.SetTexture("background.tga");
    scene.Add(background);

    // Create Player Setup
    auto player = std::make_shared<dae::GameObject>();
    player->AddComponent<dae::TransformComponent>().SetLocalPosition(50, 10, 0);
    auto& playerRender = player->AddComponent<dae::RenderComponent>();
    playerRender.SetTexture("wall.tga");
    
    auto& playerActor = player->AddComponent<dae::GameActor>();
    playerActor.SetHealth(3);
    scene.Add(player);

    
    dae::Camera::GetInstance().SetTarget(player);

   
    auto& inputManager = dae::InputManager::GetInstance();
    inputManager.BindCommand(SDL_SCANCODE_RIGHT, KeyState::Down, InputDeviceType::Keyboard,
        std::make_unique<dae::MoveCommand>(&playerActor, dae::MoveCommand::Direction::Right, 5.0f), 1);
    inputManager.BindCommand(SDL_SCANCODE_LEFT, KeyState::Down, InputDeviceType::Keyboard,
        std::make_unique<dae::MoveCommand>(&playerActor, dae::MoveCommand::Direction::Left, 5.0f), 1);
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



