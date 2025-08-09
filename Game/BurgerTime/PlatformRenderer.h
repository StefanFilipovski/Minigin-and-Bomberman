#pragma once
#include "Component.h"
#include <SDL.h>
#include <vector>

namespace dae {

    class PlatformRenderer final : public Component
    {
    public:
        PlatformRenderer(GameObject* owner,
            const std::vector<std::vector<char>>& grid,
            int gridWidth,
            int gridHeight,
            int tileSize,
            int uiHeight)
            : Component(owner)
            , m_Grid(grid)
            , m_Width(gridWidth)
            , m_Height(gridHeight)
            , m_TileSize(tileSize)
            , m_UIHeight(uiHeight)
        {
        }

        void Render() const override;

    private:
        const std::vector<std::vector<char>> m_Grid;
        const int m_Width{};
        const int m_Height{};
        const int m_TileSize{};
        const int m_UIHeight{};

        // Colors
        static constexpr SDL_Color PLATFORM_COLOR = { 0, 100, 200, 255 };
        static constexpr SDL_Color INTERSECTION_COLOR = { 100, 150, 200, 255 };
        static constexpr SDL_Color PLATE_FILL = { 220, 220, 220, 255 };
        static constexpr SDL_Color PLATE_RIM = { 180, 180, 180, 255 };

        void DrawPlatform(SDL_Renderer* r, int gx, int gy) const;
        void DrawLadderSegment(SDL_Renderer* r, int gx, int gy) const;
        void DrawLadderStartIndicator(SDL_Renderer* r, int gx, int gy) const;  // NEW
        void DrawPlatformIntersection(SDL_Renderer* r, int gx, int gy) const;
        void DrawPlate(SDL_Renderer* r, int gx, int gy) const;
    };

} // namespace dae