#pragma once
#include "ISoundSystem.h"
#include <string>

class SDLMixerSoundSystem : public ISoundSystem {
public:
    explicit SDLMixerSoundSystem(const std::string& dataPath);
    ~SDLMixerSoundSystem() override;
    void Play(sound_id id, float volume) override;
    int PlayLoop(sound_id id, float volume) override;
    void StopChannel(int channel) override;
    void Load(sound_id id, const std::string& filename);

    // Music functions
    void PlayMusic(const std::string& filename, float volume = 1.0f) override;
    void PauseMusic() override;
    void ResumeMusic() override;
    void StopMusic() override;
    void SetMusicVolume(float volume) override;

    // Mute functions
    void ToggleMute() override;
    bool IsMuted() const override;
private:
    struct Impl;
    Impl* pImpl;
};