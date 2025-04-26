#pragma once

using sound_id = unsigned short;
struct ISoundSystem {
	virtual ~ISoundSystem() = default;
	virtual void Play(sound_id id, float volume) = 0;
};
