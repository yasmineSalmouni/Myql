#pragma once
#include "Quote.hpp"
#include <limits>
#include<vector>
using namespace std; 

namespace Myql
{
    class Quote;
    class DeterministicVolQuote : public Quote
    {
    public:
        DeterministicVolQuote(const vector<double>& values = {}) : values_(values) {}

        bool isValid() const {
            return !values_.empty();
        }

        //Obligatoire pour hériter de Quote
        double value() const override{
            return values_.empty() ? 0.0 : values_.front();
        }

        double value(size_t i) const
        {
            return values_[i];
        }

        const std::vector<double>& values() const
        {
            return values_;
        }

        void setValues(const std::vector<double>& values)
        {
            if(values_ != values)
            {
                values_ = values;
                notifyObservers();
            }
        }

    private:
        vector<double> values_;
    };
}