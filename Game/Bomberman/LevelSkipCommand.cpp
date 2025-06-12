#include "LevelSkipCommand.h"
#include "LevelManager.h"
#include <iostream>

namespace dae {
    void LevelSkipCommand::Execute()
    {
        std::cout << "F1 pressed - skipping level..." << std::endl;

        // Simply queue the next level transition
        LevelManager::GetInstance().QueueLevelTransition();
    }
}