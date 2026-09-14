#include "DeterministicVolRateProcess.hpp"
#include "../market/SimpleQuote.hpp"
#include "../utilities/Date.hpp"

#include <cmath>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace Myql
{
    double DeterministicVolRateProcess::S0() const
    {
        return spot_-> value(); 
    }
    /* double DeterministicVolRateProcess::drift(double t, double S) const
    {
        return interpolatedRate(t);
    }*/

    double DeterministicVolRateProcess::drift(double t, double S) const
    {
        auto rateMap = forwardRates();

        auto it = rateMap.upper_bound(t);

        if (it == rateMap.begin())
            return it->second;

        --it;

        return it->second;
    }

    double DeterministicVolRateProcess::vol(double t, double S) const
    {
        return interpolatedVol(t);
    }
    double DeterministicVolRateProcess::expiryDate(double t) const
    {
        size_t k = static_cast<size_t>(t);

        return volCurve_->expiries_->value(k);
    }

    map<double, double> DeterministicVolRateProcess::variances() const
    {
        std::map<double, double> maturityVarianceMap;

        const auto& vols = volCurve_->volatility_->values();
        
        const auto& expiries = volCurve_->expiries_->values();
        
        /*Date today = Date::today();
        double todayDate = today.daysSince1980();*/
        double evalDate = evaluationDate_.daysSince1980();
        
        for (size_t i = 0; i < vols.size(); ++i)
        {
            double expiryDate = expiries[i];
            double Tslice = (expiryDate - evalDate) / 365.0;
            double sigma = vols[i];
            double V = sigma * sigma * Tslice;
            maturityVarianceMap[Tslice] = V;
        }
        return maturityVarianceMap;
    }

    double DeterministicVolRateProcess::interpolatedVol( double t) const
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
   double DeterministicVolRateProcess::interpolateVariance(double t) const
    {
        if (t <= 0.0)
        {
            return 0.0;
        }

        auto varianceMap = variances();

        auto it = varianceMap.lower_bound(t);

        if (it == varianceMap.begin())
        {
            return it->second;
        }

        if (it == varianceMap.end())
        {
            return std::prev(varianceMap.end())->second;
        }

        auto right = it;
        auto left  = std::prev(it);

        double T1 = left->first;
        double T2 = right->first;

        double V1 = left->second;
        double V2 = right->second;

        double w = (t - T1) / (T2 - T1);

        return V1 + w * (V2 - V1);
    }
    double DeterministicVolRateProcess::forwardVol(double tPrev,double t) const
    {
        double Vprev = interpolateVariance(tPrev);
        double Vt    = interpolateVariance(t);

        double dt = t - tPrev;

        if (dt <= 0.0)
        {
            return 0.0;
        }
    
        double var = (Vt - Vprev) / dt;

        if (var < 0.0)
        {
            var = 0.0;
        }
        double vol = std::sqrt(var);

        return vol;
    }
    
   /* map<double,double> DeterministicVolRateProcess::forwardRates() const
    {
        map<double,double> rateMap;

        const auto& dfs = rateCurve_->discountFactors_->values();

        const auto& expiryDates = rateCurve_->expiryDates_;

        //Date today = Date::today();

        double previousDF = 1.0;
        double previousT  = 0.0;

        for(size_t i = 0; i < dfs.size(); ++i)
        {
            double T = (expiryDates[i].daysSince1980() - evaluationDate_.daysSince1980()) / 365.0;

            double DF = dfs[i];

            double r =-std::log(DF / previousDF) / (T - previousT);

            rateMap[T] = r;

            previousDF = DF;
            previousT  = T;
        }
        

        return rateMap;
    }*/


    map<double,double> DeterministicVolRateProcess::forwardRates() const
    {
        map<double,double> rateMap;

        const auto& dfs =
            rateCurve_->discountFactors_->values();

        const auto& expiryDates = rateCurve_->expiryDates_;

        double DF_Prev = 1;

        std::vector<double> deltaT;

        int prevDate = evaluationDate_.daysSince1980();

        for (const auto& date : observationDates_)
        {
            int currentDate = date.daysSince1980();

            double dt =(currentDate - prevDate) / 365.0;

            deltaT.push_back(dt);

            prevDate = currentDate;
        }

        for(size_t i = 0; i < dfs.size() ; ++i)
        {
            double T1 =
                (expiryDates[i].daysSince1980()
                - evaluationDate_.daysSince1980())
                / 365.0;

            double dt = deltaT[i];

            double DF  = dfs[i];
            
           // std::cout << "DF" << DF <<std::endl;
            
            double r =-std::log(DF / DF_Prev) / dt;
            //std::cout << "r" << r <<std::endl;
            //std::cout << "dt" << dt <<std::endl;
            rateMap[T1] = r;
            
            DF_Prev = DF;
        }

        return rateMap;
    }


    /*double DeterministicVolRateProcess::interpolatedRate(double t) const
    {
        auto rateMap = forwardRates();

        auto it = rateMap.lower_bound(t);

        if (it == rateMap.begin())
        {
            return it->second;
        }

        if (it == rateMap.end())
        {
            return std::prev(rateMap.end())->second;
        }

        auto right = it;
        auto left  = std::prev(it);

        double T1 = left->first;
        double T2 = right->first;

        double r1 = left->second;
        double r2 = right->second;

        double w = (t - T1) / (T2 - T1);

        return r1 + w * (r2 - r1);
    }*/

    double DeterministicVolRateProcess::interpolatedRate(double t) const
    {
        auto rateMap = forwardRates();

        double r = rateMap.begin()->second;

        for (const auto& [T, ri] : rateMap)
        {
            if (t >= T)
            {
                r = ri;
            }
            else
            {
                break;
            }
        }

        return r;
    }


    double DeterministicVolRateProcess::evolve(double St,double dt,double dw,double tPrev, double t) const
    {
        std::cout << std::setprecision(6);
        double r =drift(t);
        std::cout << "r ="<< r <<std::endl;
        double sigma = forwardVol(tPrev,t);
        std::cout << "sigma ="<< sigma <<std::endl;
        
        //dSt = r St dt + sigma St dWt 
        /*std::cout
            <<  " tPrev=" << tPrev
            << " dt=" << dt
            << " r=" << r
            << " sigma=" << sigma
            <<  " t=" << t
            << " Z = "<< dw/ std::sqrt(dt)
            << std::endl;*/

        return St * std::exp((r - 0.5 * sigma * sigma) * dt + sigma * dw);
    }
}


        
        