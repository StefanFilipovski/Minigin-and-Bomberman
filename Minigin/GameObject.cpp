#include "GameObject.h"
#include <algorithm>
#include <stdexcept>
#include "DebugUIManager.h"

namespace dae
{
    dae::GameObject::GameObject()
    {
        DebugUIManager::GetInstance().AddGameObject(this);
    }


    GameObject::~GameObject()
    {
        if (m_pParent)
        {
            // Remove 'this' from the parent's children vector.
            auto& siblings = m_pParent->m_Children; 
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        }


        for (auto* child : m_Children)
        {
            child->m_pParent = nullptr;  
        }
        m_Children.clear();

        DebugUIManager::GetInstance().RemoveGameObject(this);
        /*std::cerr << "Destroying GameObject: " << this << std::endl;*/

    }


    void GameObject::Update(float deltaTime)
    {
        
        if (auto transform = GetComponent<TransformComponent>())
        {
            transform->UpdateWorldPosition();
        }

        // Update all components **after** updating transform
        for (auto& comp : m_pComponents)
        {
            if (!comp->IsMarkedForDeletion())
            {
                comp->Update(deltaTime);
            }
        }

        // Remove components **after** updating to prevent modifying vector while iterating
        m_pComponents.erase(
            std::remove_if(m_pComponents.begin(), m_pComponents.end(),
                [](const std::unique_ptr<Component>& comp) {
                    return comp->IsMarkedForDeletion();
                }),
            m_pComponents.end()
        );

        // Update all children **AFTER the parent is fully updated**
        for (auto* child : m_Children)
        {
            child->Update(deltaTime);
        }
    }

    void GameObject::Render() const
    {
        for (const auto& comp : m_pComponents)
        {
            if (!comp->IsMarkedForDeletion())
            {
                comp->Render();
            }
        }

        // Render all children
        for (const auto* child : m_Children)
        {
            child->Render();
        }
    }

    void GameObject::RemoveAllComponents()
    {
        for (auto& comp : m_pComponents)
        {
            comp->MarkForDeletion();
        }
    }

    void dae::GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
    {
        // 1. Validate new parent (e.g., ensure it's not this, and optionally check for cycles)
        if (parent == this)
            return;

        // Optionally, add a check to ensure 'parent' is not one of this object's descendants

        // 2. If this GameObject already has a parent, remove it from that parent's children list.
        if (m_pParent)
        {
            auto it = std::find(m_pParent->m_Children.begin(), m_pParent->m_Children.end(), this);
            if (it != m_pParent->m_Children.end())
                m_pParent->m_Children.erase(it);
        }

        // 3. Set the new parent.
        m_pParent = parent;

        // 4. If a new parent exists, add this GameObject as its child.
        if (m_pParent)
        {
            m_pParent->m_Children.push_back(this);
        }

        // 5. Update transform: if we want to keep the world position, adjust the local position accordingly.
        if (keepWorldPosition)
        {
            glm::vec3 worldPos = GetTransform().GetWorldPosition();
            if (m_pParent)
                GetTransform().SetLocalPosition(worldPos - m_pParent->GetTransform().GetWorldPosition());
            else
                GetTransform().SetLocalPosition(worldPos.x, worldPos.y, worldPos.z);
        }
        else
        {
            // Otherwise, mark the transform as dirty so it will update relative to the new parent's transform.
            GetTransform().MarkPositionDirty();
        }
    }

       
}
