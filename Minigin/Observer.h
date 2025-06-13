#pragma once
#include <vector>
#include <algorithm>

namespace dae {

    // generic type for event IDs.
    using EventID = unsigned int;

    struct Event {
        EventID id;
        int data{ 0 };
        
    };

    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void OnNotify(const Event& event) = 0;
    };

    class Subject {
    public:
        virtual ~Subject() = default;

        void AddObserver(Observer* observer) {
            m_Observers.push_back(observer);
        }

        void RemoveObserver(Observer* observer) {
            m_Observers.erase(
                std::remove(m_Observers.begin(), m_Observers.end(), observer),
                m_Observers.end());
        }

    protected:
        void Notify(const Event& event) {
            for (auto observer : m_Observers)
                observer->OnNotify(event);
        }

    private:
        std::vector<Observer*> m_Observers;
    };

} // namespace dae
