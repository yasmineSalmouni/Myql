#pragma once
#include "StochasticProcess.hpp"
#include "../market/SimpleQuote.hpp"
#include "../market/DeterministicCurveQuote.hpp"

#include <memory>
#include <vector>
#include <map>

using namespace std;

namespace Myql
{
    class StochasticProcess;
    class SimpleQuote;

    struct VolCurve
        {
            shared_ptr<DeterministicCurveQuote> volatility_;
            shared_ptr<DeterministicCurveQuote> expiryDates_;
        };

    struct RateCurve
    {
        shared_ptr<DeterministicCurveQuote> discountFactors_;
        shared_ptr<DeterministicCurveQuote> expiryDates_;
    };

    class DeterministicVolProcess : public StochasticProcess
    //dSt = r St dt + sigma(t) St dWt 
    //S(t{i+1}) =S(ti) * exp((r{i+1}-0.5 sigma(t{i+1}) sigma(t{i+1})) dt + sigma(t{i+1}) sqrt(dt) Zi) 
    {
    public:
        DeterministicVolProcess(shared_ptr<SimpleQuote> spot, shared_ptr<SimpleQuote> rate, const shared_ptr<VolCurve>& volCurve):
            spot_(spot), rate_(rate), volCurve_(volCurve) {}

        double S0() const ;
        double drift(double t=0, double S=0) const;
        double vol(double t=0, double S=0) const;
        double expiryDate(double t) const;
        map<double, double> variances() const;
        double interpolatedVol(double t) const;

        virtual double evolve(double St,double dt,double dw,double t) const;
       


    private:

        shared_ptr<SimpleQuote> spot_;
        shared_ptr<SimpleQuote> rate_;
        //shared_ptr<DeterministicCurveQuote> rate_;
        
        shared_ptr<VolCurve> volCurve_;
        shared_ptr<RateCurve> rateCurve_;
    };
}