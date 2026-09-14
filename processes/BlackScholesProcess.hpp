#pragma once
#include "StochasticProcess.hpp"
#include "../market/SimpleQuote.hpp"
#include <memory>

namespace Myql
{
    class StochasticProcess;
    class SimpleQuote;

    class BlackScholesProcess : public StochasticProcess
    //dSt = r St dt + sigma St dWt 
    //S(t+1) =S(t) * exp((r-0.5 sigma sigma) dt + sigma dt) 
    {
    public:
        BlackScholesProcess(std::shared_ptr<SimpleQuote> spot, std::shared_ptr<SimpleQuote> rate,std::shared_ptr<SimpleQuote> volatility):
            spot_(spot), rate_(rate), volatility_(volatility) {}

        double S0() const ;
        double drift(double t=0, double S=0) const;
        double vol(double t=0, double S=0) const;
        double evolve(double St, double dt, double dw, double tPrev,double t=0) const override;


    private:

        std::shared_ptr<SimpleQuote> spot_;
        std::shared_ptr<SimpleQuote> rate_;
        std::shared_ptr<SimpleQuote> volatility_;
    };
}