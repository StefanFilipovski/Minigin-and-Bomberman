#include "BombFuseState.h"
#include "BombComponent.h"
#include "SpriteSheetComponent.h"
#include "BombHideState.h"

using namespace dae;

void BombFuseState::Update(BombComponent& ctx, float dt)
{
    ctx.m_Timer += dt;                            
    if (ctx.m_Timer >= ctx.m_FuseTime)
    {
        ctx.Explode();
        if (ctx.m_pSprite)
            ctx.m_pSprite->Hide();

        ctx.TransitionTo(new BombHideState());
    }
}