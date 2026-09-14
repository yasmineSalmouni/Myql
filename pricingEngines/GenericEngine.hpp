#pragma once
#include "PricingEngine.hpp"

namespace Myql
{
    class PricingEngine;
    
    template <class ArgumentsType, class ResultsType>
    class GenericEngine : public PricingEngine
    {
    public:
        arguments* getArguments() const {return &arguments_;}
        const results* getResults() const { return &results_;}
        void reset() const {results_.reset();}
    
    protected:
        
        mutable ArgumentsType arguments_;
        mutable ResultsType results_;
    };
}