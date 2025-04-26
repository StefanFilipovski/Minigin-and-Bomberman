#include "ServiceLocator.h"
#include <cassert>

std::unique_ptr<ISoundSystem> ServiceLocator::s_soundSystem{ nullptr };

ISoundSystem& ServiceLocator::GetSoundSystem()
{
    assert(s_soundSystem && "SoundSystem not registered!");
    return *s_soundSystem;
}

void ServiceLocator::RegisterSoundSystem(std::unique_ptr<ISoundSystem> system)
{
    s_soundSystem = std::move(system);
}

