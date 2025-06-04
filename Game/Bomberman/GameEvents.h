#pragma once
#pragma once
#include "Observer.h"  

namespace GameEvents {
    // game-specific event IDs.
    constexpr dae::EventID PLAYER_HIT = 0x01;
    constexpr dae::EventID PLAYER_DIED = 0x02;
    constexpr dae::EventID POINTS_ADDED = 0x03;
    constexpr dae::EventID BOMB_EXPLODED = 0x04;
    constexpr dae::EventID ENEMY_DIED = 0x05;
  
}
