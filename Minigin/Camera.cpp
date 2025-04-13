#include "Camera.h"
#include "GameObject.h"
#include "TransformComponent.h"

namespace dae {

    void Camera::Update(float deltaTime)
    {
        (void)deltaTime; // You could use deltaTime for smoothing if desired.
        if (!m_target)
            return;

        // Get the target (player) position in world coordinates.
        const auto targetPos = m_target->GetTransform().GetWorldPosition();

        // Calculate the player's screen position given current camera offset.
        // For a 2D screen, we use only the x and y.
        glm::vec2 playerScreenPos(targetPos.x - m_offset.x, targetPos.y - m_offset.y);

        // Compute the center of the screen.
        float centerX = m_screenWidth * 0.5f;
        float centerY = m_screenHeight * 0.5f;

        // Define dead zone boundaries around the center.
        float leftBound = centerX - m_deadZoneX;
        float rightBound = centerX + m_deadZoneX;
        float topBound = centerY - m_deadZoneY;
        float bottomBound = centerY + m_deadZoneY;

        // Adjust camera offset if the player is outside the horizontal dead zone.
        if (playerScreenPos.x < leftBound)
        {
            m_offset.x = targetPos.x - leftBound;
        }
        else if (playerScreenPos.x > rightBound)
        {
            m_offset.x = targetPos.x - rightBound;
        }
        // Similarly, adjust vertically if needed:
        if (playerScreenPos.y < topBound)
        {
            m_offset.y = targetPos.y - topBound;
        }
        else if (playerScreenPos.y > bottomBound)
        {
            m_offset.y = targetPos.y - bottomBound;
        }
    }

} // namespace dae
