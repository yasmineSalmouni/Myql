#include "ELEMonteCarloEngine.hpp"
#include "../processes/DeterministicVolRateProcess.hpp"

#include <iostream>
#include <iomanip>

#include <fstream>

using namespace std;
namespace Myql
{
    ELEMonteCarloEngine::ELEMonteCarloEngine(const shared_ptr<StochasticProcess>& process, int nPaths):
    process_(process), nPaths_(nPaths), generator_(42), normal_(0.0, 1.0) {}

    vector<double> ELEMonteCarloEngine::generatePath(const std::vector<double>& Z) const
    {
        vector<double> path(arguments_.M + 1);

        double S = process_->S0();
        path[0] = S;
    
        std::vector<double> deltaT = ELE::buildDeltaT(arguments_.evaluationDate, arguments_.observationDates);
        std::vector<double> times;

        double t = 0.0;

        for(double dt : deltaT)
        {
            t += dt;
            times.push_back(t);
        }

        for(int i = 0; i < arguments_.M; ++i)
        {
            double dt = deltaT[i];
            double tPrev = (i == 0) ? 0.0 : times[i - 1];
            double t = times[i];
            double dW = sqrt(dt) * Z[i];
            S = process_->evolve(S, dt, dW,tPrev, t);
            path[i+1] = S;
        }
       
        return path;
    }

    double ELEMonteCarloEngine::computePayoff(const vector<double>& path, const vector<double>& Z) const
    {
        const double S0 = process_->S0();

        std::cout << "path[0]"<< S0 <<std::endl;

        int nij = 0; // coupon memory counter
        double payoff = 0.0;
        for (int i = 0; i < arguments_.M; ++i)
        {  
            double S = path[i + 1];
            std::cout << "path[" << i+1 << "]"<< S <<std::endl;

            //Observation coupons
            if (S >= arguments_.Bcoupon * S0)
            {
                if (arguments_.Memory)
                {
                    ++nij;
                }
                else
                {
                    payoff += arguments_.Nominal *arguments_.Coupon;
                }
            }

            if (i + 1 == arguments_.M)
            {
                if (arguments_.Memory)
                {
                    if (S >= arguments_.Bprotection * S0)
                    {
                        payoff = arguments_.Nominal * (1 + arguments_.Coupon * nij);
                    }
                    else
                    {
                        payoff = arguments_.Nominal * (S / S0 + arguments_.Coupon * nij);
                    }
                }
                else
                {
                    if (S >= arguments_.Bprotection * S0)
                    {
                        payoff += arguments_.Nominal * (1 + arguments_.Coupon * nij);
                    }
                    else
                    {
                        payoff += arguments_.Nominal * (S / S0 + arguments_.Coupon * nij);
                    }
                }
            }
        }

        return payoff;
    }

    void ELEMonteCarloEngine::calculate() const
    {
       bool converged = false;
       int N = nPaths_;
       
       
       double meanPayoff = 0.0;
       double relativeError = 0.0; //Logger 
       const double epsilon = 0.003;
       

       double discountFactor = 1.0;
       double t = 0.0;
        vector<double> deltaT = ELE::buildDeltaT(arguments_.evaluationDate, arguments_.observationDates);

        int Nmax = 100000;

        std::ofstream logFile("Z.txt");

        if (!logFile.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier\n";
        }
        std::streambuf* oldCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(logFile.rdbuf());

        std::vector<std::vector<double>> ZAll(Nmax, std::vector<double>(arguments_.M));
       
        std::uniform_real_distribution<double> uniform(0.0, 1.0);

        constexpr double PI = 3.14159265358979323846;
        

        for (int i = 0; i < Nmax; ++i)
        {
            for (int j = 0; j < arguments_.M; j += 2)
            {
                double u1 = uniform(generator_);
                double u2 = uniform(generator_);

                double r = std::sqrt(-2.0 * std::log(u1));
                double theta = 2.0 * PI * u2;

                double z1 = r * std::cos(theta);
                double z2 = r * std::sin(theta);

                ZAll[i][j] = z1;

                if (j + 1 < arguments_.M)
                    ZAll[i][j + 1] = z2;
            }
        }
        for (int i=0; i<Nmax; ++i)
        {
            for (int j=0; j<arguments_.M; ++j)
            {
                std::cout << "ZAll["<<i<<"]["<<j<<"]"<<ZAll[i][j] <<std::endl;
            }
        }
        std::ofstream logFile6("paths.txt");

        if (!logFile6.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier\n";
        }
        std::streambuf* oldCoutBuffer6 = std::cout.rdbuf();
        std::cout.rdbuf(logFile6.rdbuf());

       while (!converged)
       {
            double sumPayoff = 0.0;
            double sumPayoffSq = 0.0;

            for (int i = 0; i < N; ++i)
            {
                //Trajectoire 1
                std::vector<double> Zmain = ZAll[i];
                //std::cout << "\n========== MYQL SCENARIO " << i << " ==========\n";

                std::cout << std::setprecision(3);

                //Trajectoire 2
                std::vector<double> Zanti(arguments_.M);

                for (int j = 0; j < arguments_.M; ++j)
                {
                    Zanti[j] = -Zmain[j];
                }

                std::cout << "path1 = ";
                std::cout << std::endl;

                vector<double> path1 = generatePath(Zmain);
                
                double payoff1 = computePayoff(path1, Zmain);
        
                std::cout << "path2 = ";
                std::cout << std::endl;

                vector<double> path2 = generatePath(Zanti);

                double payoff2 = computePayoff(path2, Zanti);

                double payoff = 0.5 * (payoff1 + payoff2);
                sumPayoff += payoff;
                sumPayoffSq += payoff * payoff;
            }
            meanPayoff = sumPayoff / N;
            double variance = std::max(0.0, sumPayoffSq / N - meanPayoff * meanPayoff);
            double ecartType = sqrt(variance);
            double standardError = 1.96 * ecartType / sqrt(N);
            if (std::fabs(meanPayoff) > 1e-12)
            {
                relativeError = standardError / std::fabs(meanPayoff);
            }
            else
            {
                relativeError = std::numeric_limits<double>::infinity();
            }
           
            if (relativeError <= epsilon)
            {
                converged = true;

            }
            else
            {
                N += N / 10;
            }
        }  
       std::cout.rdbuf(oldCoutBuffer6);
    
        for (size_t i = 0; i < deltaT.size(); ++i)
        {
            t += deltaT[i];

            double r = process_->drift(t);

            discountFactor *= std::exp(-r * deltaT[i]); //AOA
        }

       std::ofstream logFile2("results.txt");

        if (!logFile2.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier\n";
        }
        std::streambuf* oldCoutBuffer2 = std::cout.rdbuf();
        std::cout.rdbuf(logFile2.rdbuf());
        std::cout << std::fixed << std::setprecision(16);

        std::cout << "Number of simulations required for convergence : " << N << std::endl;
        std::cout << "discountFactor : " << discountFactor << std::endl;

        std::cout << "\nMYQL DISCOUNT DEBUG\n";
        std::cout << std::setprecision(3);
        std::cout << "MYQL discountFactor = "
                << discountFactor
                << std::endl;

        std::cout << "MYQL meanPayoff = "
                << meanPayoff
                << std::endl;

        std::cout << "MYQL NPV = "
                << discountFactor * meanPayoff
                << std::endl;

        std::cout << "=========================================\n";
        std::cout.rdbuf(oldCoutBuffer2);

        results_.value = discountFactor * meanPayoff;
        results_.errorEstimator = relativeError;
        
        std::vector<double> times;

        double currentTime = 0.0;

        for(double dt : deltaT)
        {
            currentTime += dt;
            times.push_back(currentTime);
        }
        
        auto detProcess =std::dynamic_pointer_cast<DeterministicVolRateProcess>(process_);
        for(int i = 0; i < arguments_.M; ++i)
        {
            double tPrev =
                (i == 0) ? 0.0 : times[i - 1];

            double t = times[i];
        }
        
    }
    
}
