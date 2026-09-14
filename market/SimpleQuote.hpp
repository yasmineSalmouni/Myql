#pragma once
#include "Quote.hpp"
#include <limits>

namespace Myql
{
    class Quote;
    class SimpleQuote : public Quote
    {
    public:
        SimpleQuote(double value = std::numeric_limits<double>::quiet_NaN()) : value_(value) {}

        bool isValid() const {
            return value_ != std::numeric_limits<double>::quiet_NaN();
        }

        double value() const {
            return value_;
        }

        void setValue(double value)
        {
            if (value_ != value)
            {
                value_ = value;
                notifyObservers();
            }
        }
    private:
        double value_;
    };
}