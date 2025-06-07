#pragma once
#include <string> 

using sound_id = unsigned short;
struct ISoundSystem {
	virtual ~ISoundSystem() = default;
	virtual void Play(sound_id id, float volume) = 0;
	virtual int PlayLoop(sound_id id, float volume) = 0;  
	virtual void StopChannel(int channel) = 0;  

	// Music functions
	virtual void PlayMusic(const std::string& filename, float volume = 1.0f) = 0;
	virtual void PauseMusic() = 0;
	virtual void ResumeMusic() = 0;
	virtual void StopMusic() = 0;
	virtual void SetMusicVolume(float volume) = 0;

	// Mute functions
	virtual void ToggleMute() = 0;
	virtual bool IsMuted() const = 0;
};
