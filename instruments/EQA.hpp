#pragma once
#include "Instrument.hpp"
#include "../utilities/Date.hpp"
#include "../utilities/exceptions.hpp"
#include "../pricingEngines/GenericEngine.hpp"
#include <memory>
#include "../processes/BlackScholesProcess.hpp"
using namespace std;

namespace Myql
{
    class Instrument;
    class Date;
    class exceptions;
    class GenericEngine<class ArgumentsType, class ResultsType>;
    
    class EQA: public Instrument
    {
    public:
        class arguments;
        class engine;

        EQA(Date exerciceDate,Date evaluationDate, std::vector<Date> observationDates,double Nominal,double Coupon,double Bautocall,double Bprotection,double Bcoupon,int M,double T,
        bool Memory, string FinalPayoff, string KIType, string KIObservationType, bool CustomKIschedual, double K, double alpha);
        bool isExpired() const;
        static vector<double> buildDeltaT(const Date& evaluationDate, const std::vector<Date>& observationDates);

    private:
        Date exerciceDate_;
        Date evaluationDate_;
        std::vector<Date> observationDates_;
        double Nominal_;
        double Coupon_;
        double Bautocall_;
        double Bprotection_;
        double Bcoupon_;
        int M_;
        double T_;

        bool Memory_;
        string FinalPayoff_;
        string KIType_;
        string KIObservationType_;
        bool CustomKIschedual_;
        double K_; //Strike
        double alpha_; //Partitipation


        void setupArguments(PricingEngine::arguments* a) const;

    };

    class EQA::arguments : public PricingEngine::arguments
    {/*Les arguments sont: double Nominal, double C, double Bautocall, double Bprotection, double Bcoupon, int M, double T, , 
         
*/  public:
        Date exerciceDate;
        Date evaluationDate;
        std::vector<Date> observationDates;
        double Nominal;
        double Coupon;
        double Bautocall;
        double Bprotection;
        double Bcoupon;
        int M;
        double T;

        bool Memory;
        string FinalPayoff;
        string KIType;
        string KIObservationType;
        bool CustomKIschedual;
        double K; //Strike
        double alpha; //Partitipation

        void validate() const 
        {
            MYQL_REQUIRE(Nominal > 0.0,
               "positive nominal required");

            MYQL_REQUIRE(Coupon >= 0.0,
               "non-negative coupon required");

            MYQL_REQUIRE(Bautocall > 0.0,
               "positive autocall barrier required");

            MYQL_REQUIRE(Bprotection > 0.0,
                    "positive protection barrier required");

            MYQL_REQUIRE(Bcoupon > 0.0,
                    "positive coupon barrier required");

            MYQL_REQUIRE(M > 0,
                    "positive number of observations required");

            MYQL_REQUIRE(T > 0.0,
                    "positive maturity required");
            MYQL_REQUIRE(observationDates.size() == static_cast<size_t>(M),
            "number of observation dates inconsistent with M");

            MYQL_REQUIRE(Memory == 0.0 || Memory == 1, "bool Memory required");

            MYQL_REQUIRE(CustomKIschedual == 0.0 || CustomKIschedual == 1, "bool CustomKIschedual required");
        };
    };

    class EQA::engine : public GenericEngine<EQA::arguments, EQA::results> {};
};

/*class MonteCarloEngine : public EQA::engine
    {
    public:
        MonteCarloEngine(const std::shared_ptr<BlackScholesProcess>& process, int nPaths = 10000);
        void calculate() const;
    
    private:
        std::shared_ptr<BlackScholesProcess> process_;
        int nPaths_;
    };*/