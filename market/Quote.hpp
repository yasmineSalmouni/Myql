#pragma once
#include "../core/Observable.hpp"

namespace Myql
{
    class Observable;
    class Quote : public Observable
    {
        virtual double value() const = 0;
        virtual bool isValid() const = 0;
    };
}