#pragma once

#include "BaseEnemyComponent.h"

namespace dae {

    class DollComponent : public BaseEnemyComponent {
    public:
        DollComponent(GameObject* owner,
            float speed,
            float moveInterval,
            std::vector<std::vector<bool>> grid,
            glm::ivec2 gridSize,
            float tileSize,
            float gridOffsetY);

        ~DollComponent() override = default;

    protected:
        void UpdateAI(float dt) override;
        void InitializeAnimation() override;
        void PlayDeathAnimation() override;

    private:
        void ChooseRandomDirection();
    };
}