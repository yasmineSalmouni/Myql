#include "Instrument.hpp"
#include <iostream>
#include <stdexcept>

namespace Myql
{
    double Instrument::NPV() const 
    {
        calculate();
        return NPV_;
    }

    void Instrument::setPricingEngine(const std::shared_ptr<PricingEngine>& e)
    {
        if(engine_)
        {
            unregisterWith(engine_.get());
        }
       
        engine_ = e;
        
        if(engine_)
        {
            registerWith(engine_.get());
        }
        update();
    }

    void Instrument::calculate() const
    {
        if (isExpired())
        {
            NPV_ = 0.0;
            ErrorEstimate_  = 0.0;
            calculated_ = true;
        }
        else
        {
            LazyObject::calculate();
        }
    }

    void Instrument::performCalculations() const
    {
        if(!engine_)
        {
            throw std::runtime_error("null pricing engine");
        }
        engine_ -> reset();

        setupArguments(engine_ -> getArguments());
        engine_ -> getArguments() -> validate();
        engine_ -> calculate();
        fetchResults(engine_ -> getResults());
    }

    void Instrument::fetchResults(const PricingEngine::results* r) const
    {
        const Instrument::results* results = dynamic_cast<const Instrument::results*>(r);
        if(!results)
        {
            throw std::runtime_error("wrong result type");
        }

        NPV_ = results -> value;
        ErrorEstimate_ = results -> errorEstimator;
    }


}