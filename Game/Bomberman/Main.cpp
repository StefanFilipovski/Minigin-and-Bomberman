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

    dae::LevelLoader loader;
    // Adjust the relative path to where your level file is located.
    loader.LoadLevel("../Data/level1.txt", "Level 1");

    ////test damage keys
    //input.BindCommand(SDL_SCANCODE_H, KeyState::Down, InputDeviceType::Keyboard,
    //    std::make_unique<dae::LambdaCommand>([&pc]() { pc.TakeDamage(1); }), 1);
    //input.BindCommand(SDL_SCANCODE_D, KeyState::Down, InputDeviceType::Keyboard,
    //    std::make_unique<dae::LambdaCommand>([&pc]() { pc.TakeDamage(pc.GetHealth()); }), 1);
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



