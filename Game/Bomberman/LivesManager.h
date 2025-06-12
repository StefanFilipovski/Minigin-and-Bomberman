#pragma once
#include "Singleton.h"
#include "Observer.h"

namespace dae {
    class LivesDisplayComponent;

    class LivesManager : public Singleton<LivesManager>, public Observer {
    public:
        ~LivesManager() override;  // Add destructor

        void OnNotify(const Event& event) override;

        int GetLives() const { return m_CurrentLives; }
        void SetLives(int lives);
        void ResetLives() { SetLives(3); } // Reset to 3 lives

        void SetLivesDisplay(LivesDisplayComponent* display) { m_CurrentDisplay = display; }
        LivesDisplayComponent* GetCurrentDisplay() const { return m_CurrentDisplay; }

    private:
        friend class Singleton<LivesManager>;
        LivesManager() = default;

        int m_CurrentLives{ 3 };
        LivesDisplayComponent* m_CurrentDisplay{ nullptr };

        void UpdateDisplay();
        void HandleGameOver();
    };
}