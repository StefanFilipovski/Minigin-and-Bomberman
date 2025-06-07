#include "SDLMixerSoundSystem.h"
#include <SDL_mixer.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <SDL_log.h>
#include <chrono>

struct PlayRequest {
    sound_id id;
    float volume;
    bool loop;
    int* channelOut;
};

struct SDLMixerSoundSystem::Impl {
    std::string              dataPath;
    std::vector<Mix_Chunk*>  clips;
    std::thread              worker;
    std::mutex               mtx;
    std::condition_variable  cv;
    bool                     quitting{ false };
    std::queue<PlayRequest>  requests;
    std::queue<int>          stopRequests;
    Mix_Music* currentMusic{ nullptr };

    Impl(const std::string& dp)
        : dataPath(dp)
    {
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
       
        // Reserve space for up to 16 sounds initially
        clips.resize(16, nullptr);

        worker = std::thread([this] { this->ProcessLoop(); });
    }

    ~Impl() {
        {
            std::lock_guard lk(mtx);
            quitting = true;
            cv.notify_one();
        }
        worker.join();

        // Stop and free music
        if (currentMusic) {
            Mix_HaltMusic();
            Mix_FreeMusic(currentMusic);
        }

        Mix_CloseAudio();
        for (auto* c : clips) {
            if (c) Mix_FreeChunk(c);
        }
    }

    void LoadSound(sound_id id, const std::string& filename) {
        // Resize if necessary
        if (id >= clips.size()) {
            clips.resize(id + 1, nullptr);
        }

        // Free existing sound if any
        if (clips[id]) {
            Mix_FreeChunk(clips[id]);
            clips[id] = nullptr;
        }

        // Load new sound
        std::string fullPath = dataPath + filename;
        clips[id] = Mix_LoadWAV(fullPath.c_str());
        if (!clips[id]) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to load sound '%s': %s",
                fullPath.c_str(), Mix_GetError());
        }
    }

    void EnqueuePlay(sound_id id, float volume, bool loop, int* channelOut) {
        std::lock_guard lk(mtx);
        requests.push({ id, volume, loop, channelOut });
        cv.notify_one();
    }

    void EnqueueStop(int channel) {
        std::lock_guard lk(mtx);
        stopRequests.push(channel);
        cv.notify_one();
    }

    void ProcessLoop() {
        std::unique_lock lk(mtx);
        while (!quitting) {
            cv.wait(lk, [&] { return quitting || !requests.empty() || !stopRequests.empty(); });

            // Process stop requests first
            while (!stopRequests.empty()) {
                int channel = stopRequests.front();
                stopRequests.pop();
                lk.unlock();
                Mix_HaltChannel(channel);
                lk.lock();
            }

            // Process play requests
            while (!requests.empty()) {
                auto req = requests.front();
                requests.pop();
                lk.unlock();

                // Check if sound is loaded
                if (req.id < clips.size() && clips[req.id]) {
                    Mix_VolumeChunk(clips[req.id], int(req.volume * MIX_MAX_VOLUME));
                    int channel = Mix_PlayChannel(-1, clips[req.id], req.loop ? -1 : 0);
                    if (req.channelOut) {
                        *req.channelOut = channel;
                    }
                }

                lk.lock();
            }
        }
    }
};

SDLMixerSoundSystem::SDLMixerSoundSystem(const std::string& dataPath)
    : pImpl(new Impl(dataPath)) {
}

SDLMixerSoundSystem::~SDLMixerSoundSystem() {
    delete pImpl;
}

void SDLMixerSoundSystem::Play(sound_id id, float volume) {
    if (id >= pImpl->clips.size() || pImpl->clips[id] == nullptr) {
        return;
    }
    pImpl->EnqueuePlay(id, volume, false, nullptr);
}

int SDLMixerSoundSystem::PlayLoop(sound_id id, float volume) {
    if (id >= pImpl->clips.size() || pImpl->clips[id] == nullptr) {
        return -1;
    }

    // We need to wait for the channel to be assigned
    int channel = -1;
    pImpl->EnqueuePlay(id, volume, true, &channel);

    // Give the worker thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return channel;
}

void SDLMixerSoundSystem::StopChannel(int channel) {
    if (channel >= 0) {
        pImpl->EnqueueStop(channel);
    }
}

void SDLMixerSoundSystem::Load(sound_id id, const std::string& filename) {
    pImpl->LoadSound(id, filename);
}

void SDLMixerSoundSystem::PlayMusic(const std::string& filename, float volume) {
    // Stop current music if any
    if (pImpl->currentMusic) {
        Mix_FreeMusic(pImpl->currentMusic);
        pImpl->currentMusic = nullptr;
    }

    std::string fullPath = pImpl->dataPath + filename;
    pImpl->currentMusic = Mix_LoadMUS(fullPath.c_str());

    if (!pImpl->currentMusic) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to load music '%s': %s",
            fullPath.c_str(), Mix_GetError());
        return;
    }

    Mix_VolumeMusic(int(volume * MIX_MAX_VOLUME));
    Mix_PlayMusic(pImpl->currentMusic, -1); // -1 for infinite loop
}

void SDLMixerSoundSystem::PauseMusic() {
    Mix_PauseMusic();
}

void SDLMixerSoundSystem::ResumeMusic() {
    Mix_ResumeMusic();
}

void SDLMixerSoundSystem::StopMusic() {
    Mix_HaltMusic();
    if (pImpl->currentMusic) {
        Mix_FreeMusic(pImpl->currentMusic);
        pImpl->currentMusic = nullptr;
    }
}

void SDLMixerSoundSystem::SetMusicVolume(float volume) {
    Mix_VolumeMusic(int(volume * MIX_MAX_VOLUME));
}
