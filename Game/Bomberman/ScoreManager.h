#pragma once
#include "Singleton.h"
#include "Observer.h"

namespace dae {
    class ScoreComponent;

    class ScoreManager : public Singleton<ScoreManager>, public Observer {
    public:
        void OnNotify(const Event& event) override;

        int GetScore() const { return m_TotalScore; }
        void ResetScore();
        void AddScore(int points);

        void SetScoreDisplay(ScoreComponent* display);
        ScoreComponent* GetScoreDisplay() const { return m_CurrentDisplay; }

    private:
        friend class Singleton<ScoreManager>;
        ScoreManager() = default;

        int m_TotalScore{ 0 };
        ScoreComponent* m_CurrentDisplay{ nullptr };

        void UpdateDisplay();
    };
}