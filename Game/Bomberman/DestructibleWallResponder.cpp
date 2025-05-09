#include "DestructibleWallResponder.h"
#include "GameObject.h"
#include "SpriteSheetComponent.h"
#include "CollisionComponent.h"
#include "PlayerComponent.h"
#include "BlastResponder.h"

namespace dae {

    DestructibleWallResponder::DestructibleWallResponder(GameObject* owner)
        : m_pOwner(owner)
    {
        m_pSheet = owner->GetComponent<SpriteSheetComponent>();
        m_pCollider = owner->GetComponent<CollisionComponent>();
    }

    void DestructibleWallResponder::OnCollide(GameObject* other)
    {
        if (m_Started)
            return;

        // Only trigger on blasts
        bool isBlast = false;
        if (!other) {
            isBlast = true;  // manual calls
        }
        else if (auto* otherCC = other->GetComponent<CollisionComponent>()) {
            isBlast = dynamic_cast<BlastResponder*>(otherCC->GetResponder()) != nullptr;
        }

        if (isBlast)
        {
            m_Started = true;

            // 1) Play the 1×7 crumble animation (0.1 s per frame)
            m_pSheet->ChangeAnimationRow(
                /*totalRows=*/1,
                /*totalColumns=*/7,
                /*targetRow=*/0,
                /*newFrameDuration=*/0.1f
            );
            m_pSheet->SetLoop(false);

            // 2) After 0.5 s, hide the sprite so it disappears
            constexpr Uint32 hideDelayMs = static_cast<Uint32>(5 * 0.1f * 1000);
            SDL_AddTimer(hideDelayMs,
                [](Uint32 /*interval*/, void* param)->Uint32 {
                    // runs on the SDL timer thread, so only call the API that is threadsafe
                    // or use a user-event if you need to push to main thread.
                    // Here we assume Hide() is safe:
                    static_cast<SpriteSheetComponent*>(param)->Hide();
                    return 0;
                },
                m_pSheet);

            // 3) Disable collision so both players and further blasts pass through
            m_pCollider->SetResponder(nullptr);
        }
        else
        {
            // normal blocking behavior (e.g. player walking into it)
            if (auto* pc = other->GetComponent<PlayerComponent>()) {
                pc->RevertMove();
            }
        }
    }

} // namespace dae
