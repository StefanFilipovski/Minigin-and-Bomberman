#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <typeinfo>
#include <typeindex>

#include "Component.h"
#include "TransformComponent.h"
#include <stdexcept>
#include <iostream>

namespace dae
{
    class GameObject final
    {
    public:
        GameObject();
        ~GameObject();

        // Updates all attached components
        void Update(float deltaTime);

        // Renders all attached components
        void Render() const;

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value,
                "T must derive from Component");

            // Prevent multiple TransformComponents
            if constexpr (std::is_same_v<T, TransformComponent>)
            {
                if (GetComponent<TransformComponent>())
                    throw std::runtime_error("Error: This GameObject already has a TransformComponent!");
            }

            // 1) Create the derived component
            auto derived = std::make_unique<T>(this, std::forward<Args>(args)...);
            // 2) Keep its raw pointer for the return value
            T* rawPtr = derived.get();

            // 3) Wrap it into a unique_ptr<Component> and store
            std::unique_ptr<Component> basePtr{
                static_cast<Component*>(derived.release())
            };
            m_pComponents.push_back(std::move(basePtr));

            // 4) Return a reference to the newly created component
            return *rawPtr;
        }



        template <typename T>
        T* GetComponent() const
        {
            for (const auto& comp : m_pComponents)
            {
                if (auto casted = dynamic_cast<T*>(comp.get()))
                {
                    return casted;
                }
            }
            return nullptr;
        }


        // **Fix: Mark component for deletion instead of removing immediately**
        template <typename T>
        void RemoveComponent()
        {
            for (auto& comp : m_pComponents)
            {
                if (dynamic_cast<T*>(comp.get()) != nullptr)  
                {
                    comp->MarkForDeletion(); 
                }
            }
        }

           

        TransformComponent& GetTransform()
        {
            TransformComponent* transform = GetComponent<TransformComponent>();
            if (!transform)
            {
                throw std::runtime_error("Error: GetTransform() called on a GameObject without a TransformComponent!");
            }
            return *transform;
        }
                     

        void RemoveAllComponents(); //Helper for removal of components
        void MarkForDeletion() { m_markedForDeletion = true; }
        bool IsMarkedForDeletion() const { return m_markedForDeletion; }

        void SetParent(GameObject* parent, bool keepWorldPosition = true);
        
        GameObject* GetParent() const
        {
            if (!this)
            {
                std::cerr << "Error: GetParent() called on nullptr GameObject!" << std::endl;
                return nullptr;
            }
            return m_pParent;  
        }
              
        const std::vector<GameObject*>& GetChildren() const { return m_Children; }
        
        // Non-copyable, non-movable
        GameObject(const GameObject& other) = delete;
        GameObject(GameObject&& other) = delete;
        GameObject& operator=(const GameObject& other) = delete;
        GameObject& operator=(GameObject&& other) = delete;

    private:
        
        std::vector<std::unique_ptr<Component>> m_pComponents;
        bool m_markedForDeletion = false;

        std::vector<GameObject*> m_Children;  // Fix: Use raw pointers instead of shared_ptr

        GameObject* m_pParent = nullptr;

      
    };
}
