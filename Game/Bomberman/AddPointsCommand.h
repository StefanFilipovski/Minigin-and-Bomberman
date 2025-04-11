#pragma once
#include "Command.h"
#include "PointsSubject.h"

namespace dae {

    class AddPointsCommand : public Command {
    public:
        explicit AddPointsCommand(PointsSubject* subject)
            : m_Subject(subject) {
        }

        void Execute() override {
            if (m_Subject)
                m_Subject->AddPointsEvent();
        }

    private:
        PointsSubject* m_Subject;
    };

} 
