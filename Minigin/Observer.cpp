#include "Observer.h"

namespace dae {
    Observer::~Observer() {
        // Unsubscribe from all subjects
        auto subjectsCopy = m_Subjects;  // Copy because RemoveObserver modifies m_Subjects
        for (auto* subject : subjectsCopy) {
            if (subject) {
                subject->RemoveObserver(this);
            }
        }
    }

    void Observer::AddSubject(Subject* subject) {
        m_Subjects.push_back(subject);
    }

    void Observer::RemoveSubject(Subject* subject) {
        m_Subjects.erase(
            std::remove(m_Subjects.begin(), m_Subjects.end(), subject),
            m_Subjects.end());
    }

    Subject::~Subject() {
        // Notify all observers that this subject is being destroyed
        for (auto* observer : m_Observers) {
            if (observer) {
                observer->RemoveSubject(this);
            }
        }
    }
}