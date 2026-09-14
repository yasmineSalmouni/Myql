#pragma once
#include "Observable.hpp"
#include "Observer.hpp"

namespace Myql
{
    class Observable;
    class Observer;

    class LazyObject: public virtual Observable,                
                       public virtual Observer //Le virtual empêche l'héritage en diamant
    {
    protected:
        mutable bool calculated_; //mutable because it is modified in  calculate
        virtual void performCalculations() const = 0;
    public: 
        void update();
        virtual void calculate() const;
    };
}