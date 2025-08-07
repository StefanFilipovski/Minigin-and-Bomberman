#pragma once
#include "Singleton.h"
#include <memory>
#include <vec2.hpp>

namespace dae {
    class GameObject;

    class Camera final : public Singleton<Camera> {
    public:
        void SetTarget(std::shared_ptr<GameObject> target) { m_target = target; }
        void Update(float deltaTime);
        glm::vec2 GetOffset() const { return m_offset; }
        void SetScreenSize(int width, int height) {
            m_screenWidth = width;
            m_screenHeight = height;
        }
        void SetDeadZone(float deadZoneX, float deadZoneY) {
            m_deadZoneX = deadZoneX;
            m_deadZoneY = deadZoneY;
        }
        void Clear() { m_target.reset(); }  // Addded cleanup

    private:
        friend class Singleton<Camera>;
        Camera() = default;

        glm::vec2 m_offset{ 0.f, 0.f };
        std::weak_ptr<GameObject> m_target;  // Changed to weak_ptr
        int m_screenWidth{ 640 };
        int m_screenHeight{ 280 };
        float m_deadZoneX = 0.48f * m_screenWidth;
        float m_deadZoneY = float(m_screenHeight);
    };
}
