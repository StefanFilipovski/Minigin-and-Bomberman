#include "DebugUIManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "../3rdParty/imgui/imgui.h"
#include <algorithm>

namespace dae
{
    // Declare a static pointer outside the functions.
    static DebugUIManager* s_instance = nullptr;

    DebugUIManager& DebugUIManager::GetInstance()
    {
        if (!s_instance)
            s_instance = new DebugUIManager();
        return *s_instance;
    }

    // Shutdown function to clean up the singleton.
    void DebugUIManager::Shutdown()
    {
        delete s_instance;
        s_instance = nullptr;
    }

    void DebugUIManager::AddGameObject(GameObject* obj)
    {
        if (obj)
            m_GameObjects.push_back(obj);
    }

    void DebugUIManager::RemoveGameObject(GameObject* obj)
    {
        m_GameObjects.erase(std::remove(m_GameObjects.begin(), m_GameObjects.end(), obj), m_GameObjects.end());
    }

    void DebugUIManager::RenderUI()
    {
        ImGui::Begin("GameObject Debugger");
        ImGui::Text("Registered GameObjects:");

        for (const auto& obj : m_GameObjects)
        {
            if (!obj)
                continue;

            ImGui::Separator();
            ImGui::Text("GameObject: %p", obj);

            // Get the TransformComponent if available
            auto* transform = obj->GetComponent<TransformComponent>();
            if (transform)
            {
                auto pos = transform->GetLocalPosition();
                ImGui::Text("Local Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            }
        }

        ImGui::End();
    }
}
