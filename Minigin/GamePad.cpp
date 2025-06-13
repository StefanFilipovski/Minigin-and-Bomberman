#include "Gamepad.h"
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <Xinput.h>
#include <iostream>  
#include <winerror.h>
#include <minwinbase.h>
#pragma comment(lib, "Xinput.lib") 

namespace dae
{
   
    class Gamepad::GamepadImpl
    {
    public:
        GamepadImpl() {}

        bool IsButtonPressed(int playerIndex, int xinputButton) const
        {
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            if (XInputGetState(playerIndex, &state) == ERROR_SUCCESS)
            {
                if ((state.Gamepad.wButtons & xinputButton) != 0)
                {
                    std::cout << "[Gamepad] Button Pressed: " << xinputButton
                        << " (Player " << playerIndex << ")" << std::endl;
                    return true;
                }
            }
            return false;
        }
    };

    Gamepad::Gamepad(int playerIndex)
        : m_PlayerIndex(playerIndex), m_pImpl(new GamepadImpl())
    {
    }

    Gamepad::~Gamepad()
    {
        delete m_pImpl;
    }

   
    Gamepad::Gamepad(Gamepad&& other) noexcept
        : m_PlayerIndex(other.m_PlayerIndex), m_pImpl(other.m_pImpl)
    {
        other.m_pImpl = nullptr;
    }

    
    Gamepad& Gamepad::operator=(Gamepad&& other) noexcept
    {
        if (this != &other)
        {
            delete m_pImpl;
            m_PlayerIndex = other.m_PlayerIndex;
            m_pImpl = other.m_pImpl;
            other.m_pImpl = nullptr;
        }
        return *this;
    }

    bool Gamepad::IsButtonPressed(GamepadButton button) const
    {
        int xinputButton = 0;
        switch (button)
        {
        case GamepadButton::DPadLeft:  xinputButton = XINPUT_GAMEPAD_DPAD_LEFT;  break;
        case GamepadButton::DPadRight: xinputButton = XINPUT_GAMEPAD_DPAD_RIGHT; break;
        case GamepadButton::DPadUp:    xinputButton = XINPUT_GAMEPAD_DPAD_UP;    break;
        case GamepadButton::DPadDown:  xinputButton = XINPUT_GAMEPAD_DPAD_DOWN;  break;
        case GamepadButton::A:         xinputButton = XINPUT_GAMEPAD_A;          break;
        case GamepadButton::B:         xinputButton = XINPUT_GAMEPAD_B;          break;
        case GamepadButton::X:         xinputButton = XINPUT_GAMEPAD_X;          break;
        case GamepadButton::Y:         xinputButton = XINPUT_GAMEPAD_Y;          break;
        case GamepadButton::LB:        xinputButton = XINPUT_GAMEPAD_LEFT_SHOULDER;  break;
        case GamepadButton::RB:        xinputButton = XINPUT_GAMEPAD_RIGHT_SHOULDER; break;
        case GamepadButton::Back:      xinputButton = XINPUT_GAMEPAD_BACK;       break;
        case GamepadButton::Start:     xinputButton = XINPUT_GAMEPAD_START;      break;
        default:
            std::cerr << "[ERROR] Unknown Gamepad Button!" << std::endl;
            return false;
        }
        return m_pImpl->IsButtonPressed(m_PlayerIndex, xinputButton);
    }
}
