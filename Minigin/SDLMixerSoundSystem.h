#pragma once
#include "ISoundSystem.h"
#include <string>

class SDLMixerSoundSystem : public ISoundSystem {
public:
	explicit SDLMixerSoundSystem(const std::string& dataPath);
	~SDLMixerSoundSystem() override;
	void Play(sound_id id, float volume) override;
private:
	struct Impl;
	Impl* pImpl;
};
