#pragma once
#include <memory>
#include <stdexcept>

namespace dae
{
    class GameObject;

    class Component
    {
    public:
        explicit Component(GameObject* owner)
            : m_pOwner(owner)
        {
            if (!m_pOwner)
            {
                throw std::runtime_error("Component must have an owner upon creation!");
            }
        }

        virtual ~Component() = default;

        virtual void Update(float deltaTime) { (void)deltaTime; }
        virtual void FixedUpdate(float fixedDeltaTime) { (void)fixedDeltaTime; }
        virtual void Render() const {}

        GameObject* GetOwner() const { return m_pOwner; }  //  Owner is now immutable
        void MarkForDeletion() { m_markedForDeletion = true; }
        bool IsMarkedForDeletion() const { return m_markedForDeletion; }

    protected:
        GameObject* const m_pOwner;  // `const` so ownership never changes
    private:
        bool m_markedForDeletion = false;
    };

}
