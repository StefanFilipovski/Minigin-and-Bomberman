#include "VersusGameController.h"
#include "GameEvents.h"
#include "StartScreenLoader.h"
#include <iostream>

namespace dae {
    void VersusGameController::Update(float deltaTime)
    {
        if (m_GameEnded) {
            m_EndTimer += deltaTime;
            if (m_EndTimer >= m_EndDelay) {
                // Return to start screen
                std::cout << "Returning to start screen..." << std::endl;
                StartScreenLoader loader;
                loader.LoadStartScreen();
            }
        }
    }

    void VersusGameController::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::PLAYER_DIED && !m_GameEnded) {
            m_GameEnded = true;
            m_EndTimer = 0.0f;

            int deadPlayer = event.data;
            if (deadPlayer == 2) {
                // Player 2 (Balloon) died, Player 1 wins
                ShowWinner(1);
            }
            else {
                // Player 1 (Bomberman) died, Player 2 wins
                ShowWinner(2);
            }
        }
    }

    void VersusGameController::ShowWinner(int winner)
    {
        std::cout << "=== GAME OVER ===" << std::endl;
        std::cout << "PLAYER " << winner << " WINS!" << std::endl;
        std::cout << "Returning to start screen in " << m_EndDelay << " seconds..." << std::endl;
               
    }
}