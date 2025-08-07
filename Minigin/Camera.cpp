#include "Camera.h"
#include "GameObject.h"
#include "TransformComponent.h"

namespace dae {

    void Camera::Update(float deltaTime)
    {
        (void)deltaTime;

        // Try to lock the weak_ptr
        auto target = m_target.lock();
        if (!target)
            return;

        const auto targetPos = target->GetTransform().GetWorldPosition();
        glm::vec2 playerScreenPos(targetPos.x - m_offset.x, targetPos.y - m_offset.y);

        float centerX = m_screenWidth * 0.5f;
        float centerY = m_screenHeight * 0.5f;

        float leftBound = centerX - m_deadZoneX;
        float rightBound = centerX + m_deadZoneX;
        float topBound = centerY - m_deadZoneY;
        float bottomBound = centerY + m_deadZoneY;

        if (playerScreenPos.x < leftBound)
            m_offset.x = targetPos.x - leftBound;
        else if (playerScreenPos.x > rightBound)
            m_offset.x = targetPos.x - rightBound;

        if (playerScreenPos.y < topBound)
            m_offset.y = targetPos.y - topBound;
        else if (playerScreenPos.y > bottomBound)
            m_offset.y = targetPos.y - bottomBound;

        m_offset.x = std::max(0.f, m_offset.x);
        m_offset.y = std::max(0.f, m_offset.y);
    }

} 
