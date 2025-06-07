#include <SDL.h>
#include "InputManager.h"
#include "../3rdParty/imgui/imgui.h"
#include "../3rdParty/imgui/backends/imgui_impl_sdl2.h"
#include "../3rdParty/imgui/backends/imgui_impl_opengl3.h"
#include "Command.h"
#include <algorithm>
#include <iostream>

void dae::InputManager::BindCommand(int key, KeyState state, InputDeviceType device, std::unique_ptr<Command> command, int player)
{
    m_PlayerBindings[player].push_back(CommandBinding(key, state, device, std::move(command)));

    
    if (device == InputDeviceType::Gamepad)
    {
        if (m_Gamepads.find(player) == m_Gamepads.end())
        {
            m_Gamepads.emplace(player, Gamepad(player));
        }
    }
}

void dae::InputManager::UnbindCommand(int key, KeyState state, InputDeviceType device, int player)
{
    auto it = m_PlayerBindings.find(player);
    if (it != m_PlayerBindings.end())
    {
        auto& bindings = it->second;
        bindings.erase(std::remove_if(bindings.begin(), bindings.end(),
            [key, state, device](const CommandBinding& binding) {
                return binding.key == key && binding.state == state && binding.device == device;
            }), bindings.end());
    }
}

void dae::InputManager::ClearAllBindings()
{
    m_PlayerBindings.clear();
    m_Gamepads.clear();
    m_PreviousGamepadState.clear();
    m_GamepadEdgeTriggered.clear();
}

bool dae::InputManager::ProcessInput()
{
    SDL_Event e;
  
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            return false;

        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            KeyState state = (e.type == SDL_KEYDOWN) ? KeyState::Down : KeyState::Up;
            
            for (auto& [player, bindings] : m_PlayerBindings)
            {
                for (auto& binding : bindings)
                {
                    
                    if (binding.device == InputDeviceType::Keyboard &&
                        binding.key == e.key.keysym.scancode &&
                        binding.state == state)
                    {
                        if (binding.command)
                            binding.command->Execute();
                    }
                }
            }
        }

        ImGui_ImplSDL2_ProcessEvent(&e);
    }

    // Handle held keyboard keys (bindings with KeyState::Pressed)
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    for (auto& [player, bindings] : m_PlayerBindings)
    {
        for (auto& binding : bindings)
        {
            if (binding.device == InputDeviceType::Keyboard && binding.state == KeyState::Pressed)
            {
                if (keyboardState[binding.key])
                {
                    if (binding.command)
                        binding.command->Execute();
                }
            }
        }
    }

    // Process gamepad input for players with gamepad bindings.
    for (auto& [player, bindings] : m_PlayerBindings)
    {
       
        bool hasGamepadBinding = false;
        for (auto& binding : bindings)
        {
            if (binding.device == InputDeviceType::Gamepad)
            {
                hasGamepadBinding = true;
                break;
            }
        }
        if (hasGamepadBinding)
        {
            auto it = m_Gamepads.find(player);
            if (it != m_Gamepads.end())
            {
                Gamepad& gamepad = it->second;
                
                if (m_PreviousGamepadState.find(player) == m_PreviousGamepadState.end())
                    m_PreviousGamepadState[player] = std::unordered_map<int, bool>();
               
                if (m_GamepadEdgeTriggered.find(player) == m_GamepadEdgeTriggered.end())
                    m_GamepadEdgeTriggered[player] = std::unordered_map<int, bool>();

                for (auto& binding : bindings)
                {
                    if (binding.device == InputDeviceType::Gamepad)
                    {
                        bool currentPressed = gamepad.IsButtonPressed(static_cast<GamepadButton>(binding.key));
                        bool previousPressed = false;
                        if (m_PreviousGamepadState[player].find(binding.key) != m_PreviousGamepadState[player].end())
                            previousPressed = m_PreviousGamepadState[player][binding.key];

                        // For KeyState::Up: trigger when transitioning from pressed to not pressed.
                        if (binding.state == KeyState::Up)
                        {
                            bool alreadyTriggered = false;
                            if (m_GamepadEdgeTriggered[player].find(binding.key) != m_GamepadEdgeTriggered[player].end())
                                alreadyTriggered = m_GamepadEdgeTriggered[player][binding.key];

                            if (previousPressed && !currentPressed && !alreadyTriggered)
                            {
                                if (binding.command)
                                    binding.command->Execute();
                               
                                m_GamepadEdgeTriggered[player][binding.key] = true;
                            }
                           
                            if (!currentPressed)
                            {
                                m_GamepadEdgeTriggered[player][binding.key] = false;
                            }
                        }
                        else if (binding.state == KeyState::Down)
                        {
                            // For KeyState::Down: trigger when transitioning from not pressed to pressed.
                            bool alreadyTriggered = false;
                            if (m_GamepadEdgeTriggered[player].find(binding.key) != m_GamepadEdgeTriggered[player].end())
                                alreadyTriggered = m_GamepadEdgeTriggered[player][binding.key];

                            if (!previousPressed && currentPressed && !alreadyTriggered)
                            {
                                if (binding.command)
                                    binding.command->Execute();
                                m_GamepadEdgeTriggered[player][binding.key] = true;
                            }
                            if (!currentPressed)
                            {
                                m_GamepadEdgeTriggered[player][binding.key] = false;
                            }
                        }
                        else if (binding.state == KeyState::Pressed)
                        {
                            // For continuous input, trigger every frame as long as the button is held.
                            if (currentPressed)
                            {
                                if (binding.command)
                                    binding.command->Execute();
                            }
                        }


                        
                        m_PreviousGamepadState[player][binding.key] = currentPressed;
                    }
                }
            }
        }
    }
	return true;
}
