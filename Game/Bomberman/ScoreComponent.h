#pragma once
#include "Component.h"

namespace dae {

    class ScoreComponent : public Component {
    public:
        ScoreComponent(GameObject* owner)
            : Component(owner), m_Score(0) {
        }

        void AddPoints(int points) {
            m_Score += points;
        }

        int GetScore() const { return m_Score; }

    private:
        int m_Score;
    };

}
