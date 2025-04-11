#pragma once
#include "Singleton.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include "Command.h"
#include "GamePad.h"


enum class KeyState
{
    Up,
    Down,
    Pressed
};

// New enum to identify the input device type
enum class InputDeviceType
{
    Keyboard,
    Gamepad
};

// Modified CommandBinding to include the device type.
struct CommandBinding
{
    int key;
    KeyState state;
    InputDeviceType device;
    std::unique_ptr<Command> command;

    CommandBinding(int key, KeyState state, InputDeviceType device, std::unique_ptr<Command> command)
        : key(key), state(state), device(device), command(std::move(command)) {
    }
};

namespace dae
{
    class InputManager final : public Singleton<InputManager>
    {
    public:
      
       void BindCommand(int key, KeyState state, InputDeviceType device, std::unique_ptr<Command> command, int player);


        
        void UnbindCommand(int key, KeyState state, InputDeviceType device, int player);

        bool ProcessInput();

    private:
        
        std::unordered_map<int, std::vector<CommandBinding>> m_PlayerBindings;
       
        std::unordered_map<int, Gamepad> m_Gamepads;

      
        std::unordered_map<int, std::unordered_map<int, bool>> m_PreviousGamepadState;
        std::unordered_map<int, std::unordered_map<int, bool>> m_GamepadEdgeTriggered;

    };
}
