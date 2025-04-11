#pragma once
#include "Observer.h"
#include "GameEvents.h"  // For GameEvents::POINTS_ADDED

namespace dae {

    // A subject that dispatches point–related events.
    class PointsSubject : public Subject {
    public:
        void AddPointsEvent() {
            Event event{ GameEvents::POINTS_ADDED };
            Notify(event);
        }
    };

}
