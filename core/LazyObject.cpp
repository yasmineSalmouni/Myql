#include "LazyObject.hpp"

namespace Myql
{
    void LazyObject::update()
    {
        calculated_ = false;
        notifyObservers();
    }

    void LazyObject::calculate() const
    {
        if(!calculated_)
        {
            calculated_ = true;
            try
            {
                performCalculations();
            }
            catch(const std::exception& e)
            {
                calculated_ = false;
                throw;
            }
        }
    }



}