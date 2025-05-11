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
        for (auto& blastGO : ctx.m_Blasts)
        {
            if (auto* rc = blastGO->GetComponent<RenderComponent>())
                rc->SetScale(0.f, 0.f);
            if (auto* cc = blastGO->GetComponent<CollisionComponent>())
                cc->SetResponder(nullptr);
        }
        ctx.m_Blasts.clear();
        ctx.TransitionTo(nullptr);  
    }
}