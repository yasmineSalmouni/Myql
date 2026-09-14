#pragma once
#include "../core/Observable.hpp"

namespace Myql
{
    class Observable;

    class PricingEngine: public Observable
    {
    public:
        class arguments;
        class results;

        virtual ~PricingEngine() = default;

        virtual arguments* getArguments() const = 0;
        virtual const results* getResults() const = 0;
        virtual void reset() const = 0;
        virtual void calculate() const = 0;

    };

    class PricingEngine::arguments
    {
    public:
        virtual ~arguments() = default;
        virtual void validate() const = 0;
    };

    class PricingEngine::results
    {
    public:
        virtual ~results() = default;
        virtual void reset() = 0;
    };
}
