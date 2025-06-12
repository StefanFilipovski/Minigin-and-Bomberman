#pragma once
#include "CollisionResponder.h"

namespace dae {
    class Player2BalloonComponent;

    class Player2CollisionResponder : public CollisionResponder {
    public:
        explicit Player2CollisionResponder(Player2BalloonComponent* player2);

        void OnCollide(GameObject* other) override;

    private:
        Player2BalloonComponent* m_Player2;
    };
}