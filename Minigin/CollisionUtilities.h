#pragma once
#include <SDL.h>

namespace dae {
    bool AABBIntersect(const SDL_Rect& a, const SDL_Rect& b);
}
