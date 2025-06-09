#include "BombHideState.h"
#include "BombComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "GameObject.h"

using namespace dae;

void BombHideState::Update(BombComponent& ctx, float dt)
{
    ctx.m_HideTimer += dt;

    if (ctx.m_HideTimer >= 1.f)
    {
        // First, disable all blast collisions and visuals
        for (auto& blastGO : ctx.m_Blasts)
        {
            if (!blastGO) continue;

            // Disable rendering
            if (auto* rc = blastGO->GetComponent<RenderComponent>()) {
                rc->SetScale(0.f, 0.f);
            }

            // Disable collision
            if (auto* cc = blastGO->GetComponent<CollisionComponent>()) {
                cc->SetSize(0, 0);
                cc->SetResponder(nullptr);
            }

            // Mark for deletion
            blastGO->MarkForDeletion();
        }

        // Clear the vector
        ctx.m_Blasts.clear();

        // Mark the bomb for cleanup
        ctx.MarkForCleanup();
        ctx.GetOwner()->MarkForDeletion();

        // End state machine
        ctx.TransitionTo(nullptr);
    }
}