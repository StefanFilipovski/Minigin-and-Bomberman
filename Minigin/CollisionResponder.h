#pragma once
namespace dae {

    class GameObject; // Forward declaration

    class CollisionResponder {
    public:
        virtual ~CollisionResponder() = default;

        // Called when another object collides with this object.
        virtual void OnCollide(GameObject* other) = 0;
    };

} // namespace dae
