#pragma once
#include "Observer.h"
#include "GameEvents.h"  

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
