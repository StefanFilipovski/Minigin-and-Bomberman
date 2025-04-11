#pragma once

namespace dae
{
    
    enum class GamepadButton
    {
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        A,
        B,
        X,
        Y,
        LB,
        RB,
        Back,
        Start
    };

    class Gamepad
    {
    public:
        explicit Gamepad(int playerIndex);
        ~Gamepad();

        Gamepad(const Gamepad&) = delete;
        Gamepad& operator=(const Gamepad&) = delete;

        // Move semantics (already added from before)
        Gamepad(Gamepad&& other) noexcept;
        Gamepad& operator=(Gamepad&& other) noexcept;

        bool IsButtonPressed(GamepadButton button) const;
        int GetPlayerIndex() const { return m_PlayerIndex; }

    private:
        int m_PlayerIndex;
        class GamepadImpl;
        GamepadImpl* m_pImpl;
    };
}
