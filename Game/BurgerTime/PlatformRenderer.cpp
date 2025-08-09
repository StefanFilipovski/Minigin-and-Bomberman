#include "PlatformRenderer.h"
#include "GameObject.h"
#include "Renderer.h"
#include <iostream>

namespace dae {

    void PlatformRenderer::Render() const
    {
        SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();

        // First pass: Draw all ladders (behind platforms)
        for (int y = 0; y < m_Height; ++y)
        {
            for (int x = 0; x < m_Width; ++x)
            {
                if (y >= static_cast<int>(m_Grid.size()) ||
                    x >= static_cast<int>(m_Grid[y].size()))
                    continue;

                const char cell = m_Grid[y][x];

                // Draw ladder segments
                if (cell == 'L' || cell == 'H' || cell == '|') {
                    DrawLadderSegment(renderer, x, y);
                }
                else if (cell == '#') {
                    // Only draw a ladder in an intersection cell if it actually connects to a ladder
                    const bool hasAbove = (y > 0)
                        && (m_Grid[y - 1][x] == 'L' || m_Grid[y - 1][x] == 'H' || m_Grid[y - 1][x] == '|');
                    const bool hasBelow = (y + 1 < m_Height)
                        && (m_Grid[y + 1][x] == 'L' || m_Grid[y + 1][x] == 'H' || m_Grid[y + 1][x] == '|');

                    if (hasAbove || hasBelow) {
                        DrawLadderSegment(renderer, x, y);
                    }
                    // else: don't draw — it's just a platform tile with an intersection marker,
                    // but there is no ladder connected here in the data.
                }
            }
        }

        // Second pass: Draw all platforms (on top of ladders)
        for (int y = 0; y < m_Height; ++y)
        {
            for (int x = 0; x < m_Width; ++x)
            {
                if (y >= static_cast<int>(m_Grid.size()) ||
                    x >= static_cast<int>(m_Grid[y].size()))
                    continue;

                const char cell = m_Grid[y][x];

                switch (cell)
                {
                case '=':  // Platform only
                    DrawPlatform(renderer, x, y);
                    break;

                case '+':  // Platform with ladder starting up
                    DrawPlatform(renderer, x, y);
                    // Draw a visual indicator that ladder starts here
                    DrawLadderStartIndicator(renderer, x, y);
                    break;

                case '#':  // Platform with ladder through it
                    DrawPlatformIntersection(renderer, x, y);
                    break;

                case '>':  // Plate
                    DrawPlate(renderer, x, y);
                    break;

                default:
                    break;
                }
            }
        }
    }

    void PlatformRenderer::DrawPlatform(SDL_Renderer* renderer, int gridX, int gridY) const
    {
        SDL_Rect rect{
            gridX * m_TileSize,
            gridY * m_TileSize + m_UIHeight,
            m_TileSize,
            4  // Platform thickness at TOP of cell
        };

        // Blue platform
        SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
        SDL_RenderFillRect(renderer, &rect);

        // Highlight
        SDL_SetRenderDrawColor(renderer, 150, 200, 255, 255);
        SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
    }

    void PlatformRenderer::DrawLadderSegment(SDL_Renderer* renderer, int gridX, int gridY) const
    {
        const int x = gridX * m_TileSize;
        const int y = gridY * m_TileSize + m_UIHeight;

        // Neighbor checks
        bool hasLadderAbove = false;
        bool hasLadderBelow = false;
        bool platformAbove = false;
        bool platformBelow = false;

        if (gridY > 0) {
            const char above = m_Grid[gridY - 1][gridX];
            // Treat intersection '#' as a ladder continuation for overlap purposes
            hasLadderAbove = (above == 'L' || above == 'H' || above == '|' || above == '#');
            // Any platform cap in the above cell counts
            platformAbove = (above == '=' || above == '+' || above == '#');
        }
        if (gridY < m_Height - 1) {
            const char below = m_Grid[gridY + 1][gridX];
            hasLadderBelow = (below == 'L' || below == 'H' || below == '|' || below == '#');
            platformBelow = (below == '=' || below == '+' || below == '#');
        }

        // Compute ladder span
        constexpr int kPlatformThickness = 4; // your platform draw height
        int ladderTop = y;
        int ladderBottom = y + m_TileSize;

        // If there's a solid platform directly ABOVE but no ladder continuing,
        // extend up to the underside of that platform: tileSize - platformThickness
        if (platformAbove && !hasLadderAbove) {
            ladderTop = y - (m_TileSize - kPlatformThickness);
        }
        // If a ladder continues above, just a tiny overlap so segments blend
        else if (hasLadderAbove) {
            ladderTop = y - 1;
        }

        // If there's a solid platform directly BELOW but no ladder continuing,
        // dip into it by the cap thickness to visually touch
        if (platformBelow && !hasLadderBelow) {
            ladderBottom = y + m_TileSize + kPlatformThickness;
        }
        // If ladder continues below, tiny overlap
        else if (hasLadderBelow) {
            ladderBottom = y + m_TileSize + 1;
        }

        // Special handling for ladder-through-platform in the CURRENT cell
        const char current = m_Grid[gridY][gridX];
        if (current == '#') {
            // Start below the current cell's platform cap
            ladderTop = y + kPlatformThickness;
            ladderBottom = y + m_TileSize;
        }

        // Draw rungs
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = ladderTop; i <= ladderBottom; i += 3) {
            SDL_Rect rung{ x + 3, i, m_TileSize - 6, 1 };
            SDL_RenderFillRect(renderer, &rung);
        }

        // End caps (optional, keep your styling)
        if (!hasLadderAbove && !platformAbove && gridY > 0) {
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
            SDL_Rect topCap{ x + 2, ladderTop - 2, m_TileSize - 4, 3 };
            SDL_RenderFillRect(renderer, &topCap);
        }
        if (!hasLadderBelow && platformBelow) {
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
            SDL_Rect bottomCap{ x + 2, ladderBottom - 2, m_TileSize - 4, 3 };
            SDL_RenderFillRect(renderer, &bottomCap);
        }
    }


    void PlatformRenderer::DrawLadderStartIndicator(SDL_Renderer* renderer, int gridX, int gridY) const
    {
        // This draws a visual indicator on platforms where ladders start going up
        // Shows that you CAN'T go down through this platform
        const int x = gridX * m_TileSize;
        const int y = gridY * m_TileSize + m_UIHeight;

        // Draw small arrows or dots indicating ladder goes UP only
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow indicator

        // Draw upward arrow
        SDL_RenderDrawLine(renderer, x + m_TileSize / 2, y - 2, x + m_TileSize / 2 - 3, y + 2);
        SDL_RenderDrawLine(renderer, x + m_TileSize / 2, y - 2, x + m_TileSize / 2 + 3, y + 2);
    }

    void PlatformRenderer::DrawPlatformIntersection(SDL_Renderer* renderer, int gridX, int gridY) const
    {
        SDL_Rect rect{
            gridX * m_TileSize,
            gridY * m_TileSize + m_UIHeight,
            m_TileSize,
            4  // Platform at top of cell
        };

        // Light blue platform at intersection (can go up OR down)
        SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
        SDL_RenderFillRect(renderer, &rect);

        // Highlight
        SDL_SetRenderDrawColor(renderer, 200, 225, 255, 255);
        SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
    }

    void PlatformRenderer::DrawPlate(SDL_Renderer* renderer, int gridX, int gridY) const
    {
        const int x = gridX * m_TileSize;
        const int y = gridY * m_TileSize + m_UIHeight;

        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        SDL_Rect plate{ x + 3, y + 10, m_TileSize - 6, 8 };
        SDL_RenderFillRect(renderer, &plate);

        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        SDL_RenderDrawRect(renderer, &plate);
    }

} // namespace dae