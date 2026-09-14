#pragma once
#include <vector>

namespace Myql
{
    class Observer;

    class Observable
    {
    private:
        std::vector<Observer*> observers_;
    public:
        virtual ~Observable() = default;

        void registerObserver(Observer* observer);
        void unregisterObserver(Observer* observer);
        void notifyObservers();
    };
}