#include "HighScoreManager.h"
#include <fstream>
#include <algorithm>
#include <iostream>

namespace dae {
    void HighScoreManager::LoadHighScores()
    {
        m_HighScores.clear();
        std::ifstream file(SAVE_FILE, std::ios::binary);

        if (file.is_open()) {
            size_t count;
            file.read(reinterpret_cast<char*>(&count), sizeof(count));

            for (size_t i = 0; i < count; ++i) {
                HighScoreEntry entry;

                // Read name length and name
                size_t nameLength;
                file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                entry.name.resize(nameLength);
                file.read(&entry.name[0], nameLength);

                // Read score
                file.read(reinterpret_cast<char*>(&entry.score), sizeof(entry.score));

                m_HighScores.push_back(entry);
            }
            file.close();
            std::cout << "Loaded " << m_HighScores.size() << " high scores from file." << std::endl;
        }
        else {
            std::cout << "No existing high score file found - starting with empty list." << std::endl;
        }

        // DO NOT add default high scores - start with empty list
       
    }

    void HighScoreManager::SaveHighScores()
    {
        std::ofstream file(SAVE_FILE, std::ios::binary);

        if (file.is_open()) {
            size_t count = m_HighScores.size();
            file.write(reinterpret_cast<const char*>(&count), sizeof(count));

            for (const auto& entry : m_HighScores) {
                // Write name length and name
                size_t nameLength = entry.name.length();
                file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
                file.write(entry.name.c_str(), nameLength);

                // Write score
                file.write(reinterpret_cast<const char*>(&entry.score), sizeof(entry.score));
            }
            file.close();
            std::cout << "Saved " << m_HighScores.size() << " high scores to file." << std::endl;
        }
    }

    bool HighScoreManager::IsHighScore(int score) const
    {
        // If we have fewer than max scores, any score qualifies
        if (m_HighScores.size() < MAX_HIGH_SCORES) return true;

        // Otherwise, score must beat the lowest high score
        return score > m_HighScores.back().score;
    }

    void HighScoreManager::AddHighScore(const std::string& name, int score)
    {
        std::cout << "Adding high score: " << name << " - " << score << std::endl;

        m_HighScores.push_back({ name, score });

        // Sort by score (descending)
        std::sort(m_HighScores.begin(), m_HighScores.end(),
            [](const HighScoreEntry& a, const HighScoreEntry& b) {
                return a.score > b.score;
            });

        // Keep only top scores
        if (m_HighScores.size() > MAX_HIGH_SCORES) {
            m_HighScores.resize(MAX_HIGH_SCORES);
        }

        SaveHighScores();
    }
}