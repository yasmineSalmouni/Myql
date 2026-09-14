#include "Observable.hpp"
#include <algorithm>
#include "Observer.hpp"

namespace Myql
{
    void Observable::registerObserver(Observer* observer)
    {
        if (!observer)
        {
            return;
        }
        observers_.push_back(observer);
    }

    void Observable::unregisterObserver(Observer* observer)
    {
        if (!observer)
        {
            return;
        }
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    }

    void Observable::notifyObservers()
    {
        for (auto i=observers_.begin(); i!=observers_.end(); ++i)
        {
            (*i)->update();
        }
    }




}