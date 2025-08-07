#pragma once
#include <vector>
#include <algorithm>

namespace dae {
    using EventID = unsigned int;

    struct Event {
        EventID id;
        int data{ 0 };
    };

    class Subject;  // Forward declaration

    class Observer {
    public:
        virtual ~Observer();  // Implementation below
        virtual void OnNotify(const Event& event) = 0;

    protected:
        friend class Subject;
        void AddSubject(Subject* subject);
        void RemoveSubject(Subject* subject);

    private:
        std::vector<Subject*> m_Subjects;
    };

    class Subject {
    public:
        virtual ~Subject();

        void AddObserver(Observer* observer) {
            m_Observers.push_back(observer);
            observer->AddSubject(this);  // Track this subject in observer
        }

        void RemoveObserver(Observer* observer) {
            m_Observers.erase(
                std::remove(m_Observers.begin(), m_Observers.end(), observer),
                m_Observers.end());
            observer->RemoveSubject(this);  // Remove tracking
        }

    protected:
        void Notify(const Event& event) {
            // Make a copy in case observers modify the list during notification
            auto observersCopy = m_Observers;
            for (auto observer : observersCopy) {
                if (observer) observer->OnNotify(event);
            }
        }

    private:
        std::vector<Observer*> m_Observers;
    };
}

