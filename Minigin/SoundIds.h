#pragma once
#include "ISoundSystem.h"

namespace dae {
    namespace SoundId {
        constexpr sound_id SOUND_PLAYER_HIT = 0;
        constexpr sound_id SOUND_BOMB_PLACE = 1;
        constexpr sound_id SOUND_BOMB_EXPLODE = 2;
        constexpr sound_id SOUND_WALL_DESTROY = 3;
        constexpr sound_id SOUND_ENEMY_DIE = 4;
        constexpr sound_id SOUND_POWERUP_PICKUP = 5;
        constexpr sound_id SOUND_PLAYER_DIE = 6;
        constexpr sound_id SOUND_LEVEL_COMPLETE = 7;
        constexpr sound_id SOUND_FOOTSTEPS = 8; 
    }
}