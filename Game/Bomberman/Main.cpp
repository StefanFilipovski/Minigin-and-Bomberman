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
#include "InputManager.h"
#include "LivesDisplay.h"
#include "PointsDisplay.h"
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
#include "LevelManager.h"
#include "SoundIds.h"
#include "HighScoreManager.h"
#include "StartScreenLoader.h"




void load()
{
    // Load high scores at startup
    dae::HighScoreManager::GetInstance().LoadHighScores();

    // Initialize level manager
    dae::LevelManager::GetInstance().Initialize();

    // Load start screen instead of going directly to level 1
    dae::StartScreenLoader startLoader;
    startLoader.LoadStartScreen();
}


int main(int, char* [])
{
    try {
        dae::Minigin engine("../Data/");

        auto soundSystem = std::make_unique<SDLMixerSoundSystem>("../Data/");
        // Load all sound effects
        soundSystem->Load(dae::SoundId::SOUND_PLAYER_HIT, "bomberhit.wav");
        soundSystem->Load(dae::SoundId::SOUND_BOMB_PLACE, "placebomb.wav");
        soundSystem->Load(dae::SoundId::SOUND_BOMB_EXPLODE, "explode.wav");
        soundSystem->Load(dae::SoundId::SOUND_WALL_DESTROY, "WallDestroy.wav");
        soundSystem->Load(dae::SoundId::SOUND_ENEMY_DIE, "EnemyDie.wav");
        soundSystem->Load(dae::SoundId::SOUND_POWERUP_PICKUP, "power.wav");
        soundSystem->Load(dae::SoundId::SOUND_PLAYER_DIE, "PlayerDie.wav");
        soundSystem->Load(dae::SoundId::SOUND_LEVEL_COMPLETE, "completedlevel.wav");
        soundSystem->Load(dae::SoundId::SOUND_FOOTSTEPS, "bombermanwalk.wav");



        ServiceLocator::RegisterSoundSystem(std::move(soundSystem));
        engine.Run(load);
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
    dae::DebugUIManager::Shutdown();
    return 0;
}



