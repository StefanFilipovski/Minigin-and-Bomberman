#pragma once
#pragma once
#include "Observer.h"  // Or just declare EventID if you prefer

namespace GameEvents {
    // Define game-specific event IDs.
    constexpr dae::EventID PLAYER_DIED = 0x01;
    constexpr dae::EventID POINTS_ADDED = 0x02;
    // Add other event IDs as needed.
}
