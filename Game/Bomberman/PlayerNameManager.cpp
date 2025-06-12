#include "PlayerNameManager.h"
#include <algorithm>

namespace dae {
    void PlayerNameManager::AddLetter(char letter)
    {
        if (m_CurrentName.length() < MAX_NAME_LENGTH && letter >= 'A' && letter <= 'Z') {
            m_CurrentName += letter;
        }
    }

    void PlayerNameManager::RemoveLetter()
    {
        if (!m_CurrentName.empty()) {
            m_CurrentName.pop_back();
        }
    }

    void PlayerNameManager::ClearName()
    {
        m_CurrentName.clear();
    }

    std::string PlayerNameManager::GetDisplayName() const
    {
        std::string display = m_CurrentName;

        // Pad with underscores to show remaining slots
        while (display.length() < MAX_NAME_LENGTH) {
            display += "_";
        }

        return display;
    }
}