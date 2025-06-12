#pragma once

namespace dae {
    enum class GameState {
        Playing,
        LevelTransition,
        GameOverTransition,
        MainMenu,
        Cleanup  // Add this state
    };

    class GameStateManager {
    public:
        static GameState GetState() { return s_CurrentState; }
        static void SetState(GameState state) { s_CurrentState = state; }

        // Add this static method
        static bool IsTransitioning() {
            return s_CurrentState == GameState::LevelTransition ||
                s_CurrentState == GameState::GameOverTransition ||
                s_CurrentState == GameState::Cleanup;
        }

    private:
        inline static GameState s_CurrentState = GameState::Playing;
    };
}