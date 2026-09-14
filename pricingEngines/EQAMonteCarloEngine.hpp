#pragma once
#include "../instruments/EQA.hpp"
#include "../processes/StochasticProcess.hpp"
#include <random>
#include<cmath>
using namespace std;


namespace Myql
{
    class EQA::engine;
    
    class MonteCarloEngine : public EQA::engine
    {
    public:
        MonteCarloEngine(const std::shared_ptr<StochasticProcess>& process, int nPaths = 1000);
        vector<double> generatePath(const vector<double>& Z) const;
        double computePayoff(const std::vector<double>& path, const vector<double>& Z) const;//Compute payoff pour un scenario donné
        double computeFinalPayoff(const std::vector<double>& path, int couponCount, const vector<double>& Z) const;
        void calculate() const; //générer une trajectoire 
        //calculer son payoff
        //accumuler moyenne et variance
        //Estimer la qualité de la convergence
        //remplir results_
    
    private:
        std::shared_ptr<StochasticProcess> process_;
        int nPaths_;
        
        mutable mt19937 generator_;
        mutable normal_distribution<double> normal_;
    };
}