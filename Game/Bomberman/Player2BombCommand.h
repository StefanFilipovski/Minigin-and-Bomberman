#pragma once
#include "Command.h"
#include "Player2BalloonComponent.h"
#include "Scene.h"

namespace dae {
    class Player2BombCommand : public Command {
    public:
        Player2BombCommand(Player2BalloonComponent* player2, Scene* scene)
            : m_Player2(player2), m_Scene(scene)
        {
            assert(m_Player2 && m_Scene);
        }

        void Execute() override {
            m_Player2->PlaceBomb(*m_Scene);
        }

    private:
        Player2BalloonComponent* m_Player2;
        Scene* m_Scene;
    };
}