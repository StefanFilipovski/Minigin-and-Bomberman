#pragma once
#include "Singleton.h"
#include <vector>
#include <string>

namespace dae {
    struct HighScoreEntry {
        std::string name;
        int score;
    };

    class HighScoreManager : public Singleton<HighScoreManager> {
    public:
        void LoadHighScores();
        void SaveHighScores();

        bool IsHighScore(int score) const;
        void AddHighScore(const std::string& name, int score);

        const std::vector<HighScoreEntry>& GetHighScores() const { return m_HighScores; }

    private:
        friend class Singleton<HighScoreManager>;
        HighScoreManager() = default;

        std::vector<HighScoreEntry> m_HighScores;
        const int MAX_HIGH_SCORES = 10;
        const std::string SAVE_FILE = "highscores.dat";
    };
}