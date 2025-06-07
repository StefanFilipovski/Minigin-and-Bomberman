#include "GameController.h"
#include "LevelManager.h"

namespace dae {
    void GameController::Update(float deltaTime)
    {
        (void)deltaTime;
        // Process any pending level transitions
        LevelManager::GetInstance().ProcessPendingTransition();
    }
}