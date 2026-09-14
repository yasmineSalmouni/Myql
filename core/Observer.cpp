#include "Observer.hpp"
#include "Observable.hpp"

namespace Myql
{
    void Observer::registerWith(Observable* observable)
    {
        if (observable)
        {
            observable->registerObserver(this);
        }
    }

    void Observer::unregisterWith(Observable* observable)
    {
        if(observable)
        {
            observable -> unregisterObserver(this);
        }
    }
}