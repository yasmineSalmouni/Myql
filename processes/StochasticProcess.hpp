#pragma once

namespace Myql
{

class StochasticProcess
{
public:

    virtual ~StochasticProcess() = default; 

    //S0
    virtual double S0() const = 0;

    //μ(t,S)=r 
    virtual double drift(
        double t=0,
        double S=0) const = 0;

    //σ(t,S)=σ 
    virtual double vol(
        double t=0,
        double S=0) const = 0;

    //S(t+dt)
    virtual double evolve(
    double St,
    double dt,
    double dw,
    double tPrev,
    double t=0) const = 0;
};

}
