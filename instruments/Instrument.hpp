#pragma once
#include <memory>

#include "../core/Observer.hpp"
#include "../core/LazyObject.hpp"
#include "../pricingEngines/PricingEngine.hpp"


namespace Myql
{
    class LazyObject;

    class Instrument : public LazyObject
    {
    public:
        class results;

        virtual ~Instrument()=default;
        
        virtual double NPV() const;
        virtual bool isExpired() const=0;
        void setPricingEngine(const std::shared_ptr<PricingEngine>& e) ;


    protected:
        std::shared_ptr<PricingEngine> engine_;
        mutable double NPV_;
        mutable double ErrorEstimate_ = 0.0;
        
        void calculate() const;
        void performCalculations() const;
        virtual void setupArguments(PricingEngine::arguments* args) const = 0;
        virtual void fetchResults(const PricingEngine::results* r) const;

    };

    class Instrument::results : public PricingEngine::results
    {
    public:
        mutable double value;
        mutable double errorEstimator;

        void reset() {
            value = errorEstimator = 0.0;
        };
    };
}