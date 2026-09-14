#include "DeterministicVolProcess.hpp"
#include "../market/SimpleQuote.hpp"
#include "../utilities/Date.hpp"

#include <cmath>
#include <chrono>
#include <stdexcept>

namespace Myql
{
    double DeterministicVolProcess::S0() const
    {
        return spot_-> value(); 
    }
     double DeterministicVolProcess::drift(double t, double S) const
    {
        //size_t k = static_cast<size_t>(t);
        return rate_-> value();
    }
    double DeterministicVolProcess::vol(double t, double S) const
    {
        return interpolatedVol(t);
    }
    double DeterministicVolProcess::expiryDate(double t) const
    {
        size_t k = static_cast<size_t>(t);

        return volCurve_->expiryDates_->value(k);
    }

    map<double, double> DeterministicVolProcess::variances() const
    {
        std::map<double, double> maturityVarianceMap;

        const auto& vols = volCurve_->volatility_->values();
        
        const auto& expiries = volCurve_->expiryDates_->values();
        Date today = Date::today();
        double todayDate = today.daysSince1980();
        
        for (size_t i = 0; i < vols.size(); ++i)
        {
            double expiryDate = expiries[i];
            double Tslice = (expiryDate - todayDate) / 365.0;
            double sigma = vols[i];
            double V = sigma * sigma * Tslice;
            maturityVarianceMap[Tslice] = V;
        }
        return maturityVarianceMap;
    }

    double DeterministicVolProcess::interpolatedVol(double t) const
    {
        auto varianceMap = variances();

        if (varianceMap.empty())
        {
            throw std::runtime_error("empty variance curve");
        }

        auto it = varianceMap.lower_bound(t);

        // extrapolation gauche
        if (it == varianceMap.begin())
        {
            return std::sqrt(it->second / it->first);
        }

        // extrapolation droite
        if (it == varianceMap.end())
        {
            auto last = std::prev(varianceMap.end());

            return std::sqrt(last->second / last->first);
        }

        auto right = it;
        auto left  = std::prev(it);

        double T1 = left->first;
        double T2 = right->first;

        double V1 = left->second;
        double V2 = right->second;

        double w = (t - T1) / (T2 - T1);

        double V = V1 + w * (V2 - V1);

        return std::sqrt(V / t);
    }


    double DeterministicVolProcess::evolve(double St,double dt,double dw, double t) const
    {
        double r =drift();
        double sigma = vol(t);
        //dSt = r St dt + sigma St dWt 
        return St * std::exp((r - 0.5 * sigma * sigma) * dt + sigma * dw);
    }
}


        
        