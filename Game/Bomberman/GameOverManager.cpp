#include "GameOverManager.h"
#include "ScoreManager.h"  
#include "HighScoreManager.h"
#include <iostream>

namespace dae {
    void GameOverManager::TriggerGameOver()
    {
        int finalScore = ScoreManager::GetInstance().GetScore();  // Get from ScoreManager
        std::cout << "Game Over! Final Score: " << finalScore << std::endl;

        if (HighScoreManager::GetInstance().IsHighScore(finalScore)) {
            std::cout << "New High Score!" << std::endl;
            HighScoreManager::GetInstance().AddHighScore("AAA", finalScore);
        }

        // Display high scores
        std::cout << "\n=== HIGH SCORES ===" << std::endl;
        const auto& highScores = HighScoreManager::GetInstance().GetHighScores();
        for (size_t i = 0; i < highScores.size(); ++i) {
            std::cout << i + 1 << ". " << highScores[i].name << " - " << highScores[i].score << std::endl;
        }
    }
}