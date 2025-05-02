#pragma once
#include "Singleton.h"
#include <memory>
#include <vec2.hpp>

namespace dae {

  
    class GameObject;

    class Camera : public Singleton<Camera> {
    public:
        // Set the target to follow
        void SetTarget(std::shared_ptr<GameObject> target) { m_target = target; }
        // Update camera position 
        void Update(float deltaTime);
        // Get the current offset 
        glm::vec2 GetOffset() const { return m_offset; }
        void SetScreenSize(int width, int height) { m_screenWidth = width; m_screenHeight = height; }
        void SetDeadZone(float deadZoneX, float deadZoneY) { m_deadZoneX = deadZoneX; m_deadZoneY = deadZoneY; }
        
    private:
        glm::vec2 m_offset{ 0.f, 0.f };
        std::shared_ptr<GameObject> m_target;
        // Screen dimensions can be set from configuration or during initialization.
        int m_screenWidth{ 640 };
        int m_screenHeight{ 480 };

        // dead zone margins
        float m_deadZoneX = 0.25f * m_screenWidth; 
        float m_deadZoneY = 0.25f * m_screenHeight;

        // smoothing factors, etc.
    };

} 
