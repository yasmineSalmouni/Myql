#pragma once
#include "StochasticProcess.hpp"
#include "../market/SimpleQuote.hpp"
#include "../market/DeterministicCurveQuote.hpp"
#include "../utilities/Date.hpp"

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
            shared_ptr<DeterministicCurveQuote> expiries_;
        };

    struct RateCurve
    {
        shared_ptr<DeterministicCurveQuote> discountFactors_; //P(0,t)
        std::vector<Date> expiryDates_; 
    };

    class DeterministicVolRateProcess : public StochasticProcess
    //dSt = r St dt + sigma(t) St dWt 
    //S(t{i+1}) =S(ti) * exp((r{i+1}-0.5 sigma(t{i+1}) sigma(t{i+1})) dt + sigma(t{i+1}) sqrt(dt) Zi) 
    {
    public:
        DeterministicVolRateProcess(shared_ptr<SimpleQuote> spot, shared_ptr<RateCurve> rateCurve, const shared_ptr<VolCurve>& volCurve, const Date& evaluationDate, std::vector<Date>& observationDates):
            spot_(spot), volCurve_(volCurve), rateCurve_(rateCurve), evaluationDate_(evaluationDate), observationDates_(observationDates){};

        double S0() const ;
        double drift(double t=0, double S=0) const;
        double vol(double t=0, double S=0) const;
        double expiryDate(double t) const;
        map<double, double> variances() const;
        double interpolatedVol(double t) const;
        double interpolateVariance(double t) const;
        double forwardVol(double tPrev, double t) const;


        map<double,double> forwardRates() const;
        double interpolatedRate(double t) const;
        double evolve(double St,double dt, double dw, double tPrev,double t=0) const override;
       


    private:

        shared_ptr<SimpleQuote> spot_;
        
        shared_ptr<VolCurve> volCurve_;
        shared_ptr<RateCurve> rateCurve_;

        Date evaluationDate_;
        std::vector<Date> observationDates_;
    };
}