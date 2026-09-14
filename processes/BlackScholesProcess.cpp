#include "BlackScholesProcess.hpp"
#include "../market/SimpleQuote.hpp"

#include <cmath>

namespace Myql
{
    double BlackScholesProcess::S0() const
    {
        return spot_-> value(); 
    }
     double BlackScholesProcess::drift(double t, double S) const
    {
        return rate_-> value();
    }
    double BlackScholesProcess::vol(double t, double S) const
    {
        return volatility_ -> value();
    }
    double BlackScholesProcess::evolve(double St,double dt,double dw, double tPrev, double t) const
    {
        double r =drift();
        double sigma = vol();
        //dSt = r St dt + sigma St dWt 
        return St * std::exp((r - 0.5 * sigma * sigma) * dt + sigma * dw);
    }
}

        
        