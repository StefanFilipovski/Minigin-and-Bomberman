#pragma once
#include "Singleton.h"

namespace dae {
    class GameOverManager : public Singleton<GameOverManager> {
    public:
        void TriggerGameOver();

    private:
        friend class Singleton<GameOverManager>;
        GameOverManager() = default;
    };
}