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

void load()
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene("Collision Blocking Test");

    // --- Set Up a Static Wall Object ---
    auto wall = std::make_shared<dae::GameObject>();
    wall->AddComponent<dae::TransformComponent>().SetLocalPosition(520, 100, 0);
    auto& wallRender = wall->AddComponent<dae::RenderComponent>();
    wallRender.SetTexture("wall.tga"); // Ensure this image exists
    auto& wallCollision = wall->AddComponent<dae::CollisionComponent>();
    wallCollision.SetSize(225, 225);
    // Set the wall's collision responder (using unique_ptr)
    wallCollision.SetResponder(std::make_unique<dae::StaticWallResponder>());
    scene.Add(wall);

    // --- Set Up a Dynamic Player Object ---
    auto player = std::make_shared<dae::GameObject>();
    player->AddComponent<dae::TransformComponent>().SetLocalPosition(120, 120, 0);
    auto& playerRender = player->AddComponent<dae::RenderComponent>();
    playerRender.SetTexture("wall.tga"); // Ensure this image exists

    // Attach a collision component to the player
    auto& playerCollision = player->AddComponent<dae::CollisionComponent>();
    playerCollision.SetSize(225, 225); // Set a size suitable for your player

    // Attach a player component (manages movement, etc.)
    auto& playerComp = player->AddComponent<dae::PlayerComponent>();
    playerComp.SetHealth(3);
    scene.Add(player);

    // --- Bind a Key for Movement ---
    auto& inputManager = dae::InputManager::GetInstance();
    inputManager.BindCommand(SDL_SCANCODE_RIGHT, KeyState::Down, InputDeviceType::Keyboard,
        std::make_unique<dae::LambdaCommand>([player]() {
            if (auto* playerComp = player->GetComponent<dae::PlayerComponent>()) {
                playerComp->BeginMove(); // Save the current position before moving.
                playerComp->Move(10, 0);   // Attempt to move right by 10 pixels.
            }
            }), 1);

    // The CollisionManager is expected to call CheckCollisions() during the engine update, which will
    // then detect the collision between the wall and the player.
}



int main(int, char* []) {
    dae::Minigin engine("../Data/");
    engine.Run(load);

    dae::DebugUIManager::Shutdown();
    return 0;
}


