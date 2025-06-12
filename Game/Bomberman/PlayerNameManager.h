#pragma once
#include "Singleton.h"
#include <string>

namespace dae {
    class PlayerNameManager : public Singleton<PlayerNameManager> {
    public:
        void AddLetter(char letter);
        void RemoveLetter();
        void ClearName();

        bool IsNameComplete() const { return m_CurrentName.length() == 3; }
        const std::string& GetPlayerName() const { return m_CurrentName; }
        std::string GetDisplayName() const;

    private:
        friend class Singleton<PlayerNameManager>;
        PlayerNameManager() = default;

        std::string m_CurrentName;
        static constexpr int MAX_NAME_LENGTH = 3;
    };
}