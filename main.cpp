#include "core/Observable.hpp"
#include "core/Observer.hpp"
#include "core/LazyObject.hpp"

#include "instruments/Instrument.hpp"
#include "instruments/EQA.hpp"
#include "instruments/ELE.hpp"

#include "market/Quote.hpp"
#include "market/SimpleQuote.hpp"
#include "market/DeterministicCurveQuote.hpp"

#include "pricingEngines/PricingEngine.hpp"
#include "pricingEngines/GenericEngine.hpp"
#include "pricingEngines/EQAMonteCarloEngine.hpp"
#include "pricingEngines/ELEMonteCarloEngine.hpp"

#include "processes/StochasticProcess.hpp"
#include "processes/BlackScholesProcess.hpp"
#include "processes/DeterministicVolRateProcess.hpp"

#include "utilities/Date.hpp"
#include "utilities/exceptions.hpp"

#include <iostream>
#include <chrono>
#include <fstream>
using namespace Myql;
using namespace std;

#include <iostream>
#include <fstream>


int main()
{
    //EQA
    std::ofstream logFileOG("output.txt");

    if (!logFileOG.is_open())
    {
        std::cerr << "Impossible d'ouvrir le fichier\n";
        return 1;
    }

    // Redirection de std::cout
    std::streambuf* oldCoutBufferOG = std::cout.rdbuf();
    std::cout.rdbuf(logFileOG.rdbuf());

    
    
    //EQA Contract data (arguments) (Mx)
    //Date today = Date::today();
    //double todayDate = today.daysSince1980();
    
    Date evalDate(10, 01, 2013);
    Date evaluationDate (evalDate);
    Date exerciceDate(12, 01, 2015);
    std::vector<Date> observationDates =
    {
        Date(14, 01, 2013),
        Date(15, 04, 2013),
        Date(15, 07, 2013),
        Date(14, 10, 2013),
        Date(14, 01, 2014),
        Date(14, 04, 2014),
        Date(14, 07, 2014),
        Date(14, 10, 2014),
        Date(12, 01, 2015)

    };

    EQA product(
        exerciceDate,
        evaluationDate,
        observationDates,
        10000000,     // Nominal
        0.06,       // Coupon
        1.00,       // Bautocall
        0.70,       // Bprotection
        0.60,       // Bcoupon
        9,          // M
        2.0,        // T

        false,       // Memory
        "Put",   // FinalPayoff
        "Down & In", //KIType
        "European", //KIObservationType
        false,      //CustomKISchedule
        1,        // K
        1.0         // alpha
    );
    
    //Market Data (Mx)
    auto spot = make_shared<SimpleQuote>(300.0);
    //auto rate = make_shared<SimpleQuote>(0.02);
    //auto vol = make_shared<SimpleQuote>(0.20);
    //auto DeterministicVol = make_shared<DeterministicCurveQuote>(vector{0.20, 0.30, 0.40, 0.50, 0.60});

    // Vols ATM
    auto vols =
        std::make_shared<DeterministicCurveQuote>(
            std::vector<double>{
                0.275206,
                0.279599,  
                0.286724, 
                0.310523,  
                0.31493,  
                0.323701,   
                0.330526,
                0.331252,
                0.336079,
                0.340088,
                0.343631,
                0.342674,
                0.345734,
                0.34829,
                0.349835,
                0.351065,
                0.352129,
                0.353067,
                0.35419,
                0.35511,
                0.355931,
                0.356678,
                0.356678,
                0.356678
            });
    auto expiries =
    std::make_shared<DeterministicCurveQuote>(
        std::vector<double>{
            12071,
            12099,
            12127,
            12225,
            12316,
            12407,
            12498,
            12589,
            12680,
            12771,
            12862,
            12953,
            13044,
            13135,
            13226,
            13317,
            13410,
            13499,
            13590,
            13681,
            13772,
            13863,
            14045,
            14234
        });

    auto volCurve = make_shared<VolCurve>();
    volCurve->volatility_ = vols;
    volCurve->expiries_ = expiries;

    auto discountFactors =
    std::make_shared<DeterministicCurveQuote>(
        std::vector<double>{
            0.998905,
            0.9868,   
            0.974575,   
            0.962502,   
            0.949887,
            0.938056,   
            0.925418,   
            0.912527,
            0.899813
        });
    
    auto rateCurve = std::make_shared<RateCurve>();

    rateCurve->discountFactors_ = discountFactors;

    rateCurve->expiryDates_ = observationDates;

    //BS process
    //auto process = make_shared<BlackScholesProcess>(spot,rate,vol);

    auto DeterministicProcess =  make_shared<DeterministicVolRateProcess>(spot,rateCurve,volCurve, evaluationDate, observationDates);

    //Mc engine
    auto engine = make_shared<MonteCarloEngine>( DeterministicProcess, 1000);
    
    product.setPricingEngine(engine);
    
    cout<<"Start computation"<<endl;
    auto start = chrono::high_resolution_clock::now();
    double npv = product.NPV();
    auto end = chrono::high_resolution_clock::now();
    cout<<"End computation"<<endl;
    cout << "NPV =" << npv << endl;
    auto duration = end - start;
    cout << "Computation time:" << chrono::duration<double>(duration).count() << "seconds" << std::endl;

    // Restauration de std::cout
    std::cout.rdbuf(oldCoutBufferOG);

    std::cout << "Affiché sur la console" << std::endl;

    //ELE
    
    /*Date evalDateELE(10, 01, 2013);
    Date evaluationDateELE (evalDateELE);
    Date exerciceDateELE(14, 01, 2015);
    
    std::vector<Date> observationDatesELE = 
    {
        Date(31, 1, 2013),
        Date(28, 2, 2013),
        Date(28, 3, 2013),
        Date(29, 4, 2013),
        Date(31, 5, 2013),
        Date(1, 7, 2013),
        Date(31, 7, 2013),
        Date(2, 9, 2013),
        Date(30, 9, 2013),
        Date(31, 10, 2013),
        Date(2, 12, 2013),
        Date(30, 12, 2013),
        Date(31, 1, 2014),
        Date(28, 2, 2014),
        Date(31, 3, 2014),
        Date(29, 4, 2014),
        Date(2, 6, 2014),
        Date(30, 6, 2014),
        Date(31, 7, 2014),
        Date(1, 9, 2014),
        Date(30, 9, 2014),
        Date(31, 10, 2014),
        Date(1, 12, 2014),
        Date(30, 12, 2014)
};

    ELE productELE(
        exerciceDateELE,
        evaluationDateELE,
        observationDatesELE,
        100000000,     // Nominal
        0.06,       // Coupon
        0.70,       // Bprotection
        1,       // Bcoupon
        24,          // M
        2.0,        // T
        true       // Memory
    );

    //Market Data (Mx)
    auto spotELE = make_shared<SimpleQuote>(5.16);
    //auto rate = make_shared<SimpleQuote>(0.02);
    //auto vol = make_shared<SimpleQuote>(0.20);
    //auto DeterministicVol = make_shared<DeterministicCurveQuote>(vector{0.20, 0.30, 0.40, 0.50, 0.60});

    // Vols ATM

    auto volsELE =
        std::make_shared<DeterministicCurveQuote>(
            std::vector<double>{
                0.2, 0.2, 0.2, 0.2, 0.2,
                0.2, 0.2, 0.2, 0.2, 0.2,
                0.2, 0.2, 0.2, 0.2, 0.2,
                0.2, 0.2, 0.2, 0.2, 0.2
            });
    auto expiriesELE =
    std::make_shared<DeterministicCurveQuote>(
        std::vector<double>{
            12071,
            12099,
            12127,
            12162,
            12225,
            12316,
            12407,
            12498,
            12589,
            12680,
            12771,
            13135,
            13499,
            13863,
            14234,
            14598,
            14962,
            15326,
            15690,
            18616
        });

    auto volCurveELE = make_shared<VolCurve>();
    volCurveELE->volatility_ = volsELE;
    volCurveELE->expiries_ = expiriesELE;

    auto discountFactorsELE =
    std::make_shared<DeterministicCurveQuote>(
        std::vector<double>{
            0.995217,
            0.991407,
            0.987071,
            0.983157,
            0.97991,
            0.974842,
            0.970844,
            0.966466,
            0.962766,
            0.958686,
            0.954493,
            0.950708,
            0.94646,
            0.942609,
            0.938333,
            0.93418,
            0.929598,
            0.925697,
            0.921367,
            0.916883,
            0.912808,
            0.908441,
            0.904061,
            0.899813
        });
    
    auto rateCurveELE = std::make_shared<RateCurve>();

    rateCurveELE->discountFactors_ = discountFactorsELE;

    rateCurveELE->expiryDates_ = observationDatesELE;

    //BS process

    auto DeterministicProcessELE =  make_shared<DeterministicVolRateProcess>(spotELE,rateCurveELE,volCurveELE, evaluationDateELE, observationDatesELE);

    //Mc engine
    auto engineELE = make_shared<ELEMonteCarloEngine>( DeterministicProcessELE, 1000);
    
    productELE.setPricingEngine(engineELE);
    
    cout<<"Start computation"<<endl;
    auto startELE = chrono::high_resolution_clock::now();
    double npvELE = productELE.NPV();
    auto endELE = chrono::high_resolution_clock::now();
    cout<<"End computation"<<endl;
    cout << "NPV ELE =" << npvELE << endl;
    auto durationELE = endELE - startELE;
    cout << "Computation time:" << chrono::duration<double>(durationELE).count() << "seconds" << std::endl;

    // Restauration de std::cout
    std::cout.rdbuf(oldCoutBufferOG);

    std::cout << "Affiché sur la console" << std::endl;*/

    return 0;
}


