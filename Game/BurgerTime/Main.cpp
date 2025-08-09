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
#include <DebugUIManager.h>
#include "CollisionManager.h"  
#include "CollisionComponent.h" 
#include "CollisionResponder.h" 
#include "Camera.h"
#include "ServiceLocator.h"
#include "SDLMixerSoundSystem.h"
#include "SoundIds.h"
#include "LevelLoader.h"


void load()
{
    dae::LevelLoader LoadLevel{};
    LoadLevel.LoadLevel("../Data/BurgerTimeLevel1.txt", "scene1");
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
 /*   dae::DebugUIManager::Clear();*/
    return 0;
}



