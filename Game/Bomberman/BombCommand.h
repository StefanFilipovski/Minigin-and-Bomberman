#pragma once
#include "Command.h"
#include "PlayerComponent.h"
#include "Scene.h"

namespace dae {
    class BombCommand : public Command {
    public:
        BombCommand(PlayerComponent* pc, Scene* scene)
            : m_Player(pc), m_Scene(scene)
        {
            assert(m_Player && m_Scene);
        }
        void Execute() override {
            m_Player->PlaceBomb(*m_Scene);
        }
    private:
        PlayerComponent* m_Player;
        Scene* m_Scene;
    };
}