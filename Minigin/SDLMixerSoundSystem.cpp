#include "SDLMixerSoundSystem.h"
#include <SDL_mixer.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <SDL_log.h>

struct SDLMixerSoundSystem::Impl {
    std::string              dataPath;
    std::vector<Mix_Chunk*>  clips;
    std::thread              worker;
    std::mutex               mtx;
    std::condition_variable  cv;
    bool                     quitting{ false };
    std::queue<std::pair<sound_id, float>> requests;

    Impl(const std::string& dp)
        : dataPath(dp)
    {
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        // 0 = player-hit SFX
        clips.resize(1);
        std::string hitFile = dataPath + "BombermanHitSound.wav";
        clips[0] = Mix_LoadWAV(hitFile.c_str());
        if (!clips[0]) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to load hit SFX '%s': %s",
                hitFile.c_str(), Mix_GetError());
        }

        worker = std::thread([this] { this->ProcessLoop(); });
    }

    ~Impl() {
        {
            std::lock_guard lk(mtx);
            quitting = true;
            cv.notify_one();
        }
        worker.join();
        Mix_CloseAudio();
        for (auto* c : clips) Mix_FreeChunk(c);
    }

    void Enqueue(sound_id id, float volume) {
        std::lock_guard lk(mtx);
        requests.emplace(id, volume);
        cv.notify_one();
    }

    void ProcessLoop() {
        std::unique_lock lk(mtx);
        while (!quitting) {
            cv.wait(lk, [&] { return quitting || !requests.empty(); });
            while (!requests.empty()) {
                auto [id, vol] = requests.front(); requests.pop();
                lk.unlock();
                Mix_VolumeChunk(clips[id], int(vol * MIX_MAX_VOLUME));
                Mix_PlayChannel(-1, clips[id], 0);
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

// SDLMixerSoundSystem.cpp

void SDLMixerSoundSystem::Play(sound_id id, float volume) {
    // don’t even queue up anything if that slot is invalid or not loaded
    if (id >= pImpl->clips.size() || pImpl->clips[id] == nullptr) {
        return;
    }
    pImpl->Enqueue(id, volume);
}

