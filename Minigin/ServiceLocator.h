#include <memory>
#include "ISoundSystem.h"

struct ServiceLocator {
	static ISoundSystem& GetSoundSystem();
	static void RegisterSoundSystem(std::unique_ptr<ISoundSystem> sys);
private:
	static std::unique_ptr<ISoundSystem> s_soundSystem;
};
