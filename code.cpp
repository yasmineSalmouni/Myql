#include <ql/quantlib.hpp>
#include <iostream>

using namespace QuantLib;

int main()
{
    Date today(27, July, 2026);
    Settings::instance().evaluationDate() = today;
    // Produit : classe qui contient seulament la description du contrat
    //Building an option require only the specifications of it's contract, Market data will be selected and passed later depending on the calculation methods
    // VanillaOption(const ext::shared_ptr<StrikedTypePayoff>&,const ext::shared_ptr<Exercise>&);
    ext::shared_ptr payoff = ext::make_shared<PlainVanillaPayoff>(Option::Call, 100.0 );
    //const ext::shared_ptr<PlainVanillaPayoff>& payoff(Option::Call, 100.); //Au lieu de faire : StrikedTypePayoff* payoff, on fait : ext::shared_ptr<StrikedTypePayoff> payoff; Le shared_ptr gère automatiquement la mémoire. Quand plus personne n'utilise l'objet, il est détruit automatiquement.
    Date date(27, December, 2026);
    auto Exercice = ext::make_shared<EuropeanExercise>(date);
    //const ext::shared_ptr<EuropeanExercise>& Exercice (date);
    EuropeanOption option(payoff, Exercice);

    /****************************************************/
    //Market data changes so not an arg of the instrument constructor
    auto u = ext::make_shared<SimpleQuote> (100.); //Spot
    auto r = ext::make_shared<SimpleQuote> (0.01); //risk free rate
    auto sigma =  ext::make_shared<SimpleQuote> (0.2); //vol constante

    //Création des courbes de market data
    //Courbe de taux plate r = 1% pour toutes les maturités
    auto riskFreeCurve = ext::make_shared<FlatForward> (0, TARGET(), Handle<Quote>(r),Actual360());


    /*Reference date + Quote +  daycounter
    FlatForward(const Date& referenceDate, Handle<Quote> forward,const DayCounter& dayCounter,Compounding compounding = Continuous,Frequency frequency = Annual);

    rate harcoded
    FlatForward(const Date& referenceDate, Rate forward,const DayCounter& dayCounter, Compounding compounding = Continuous,Frequency frequency = Annual);

    reference value est remplacée par nombre of settlement days + calendar
    FlatForward(Natural settlementDays,const Calendar& calendar,Handle<Quote> forward,const DayCounter& dayCounter,Compounding compounding = Continuous,Frequency frequency = Annual);

     reference value est remplacée par nombre of settlement days + calendar + rate hardcoded 
    FlatForward(Natural settlementDays,const Calendar& calendar,Rate forward,const DayCounter& dayCounter,Compounding compounding = Continuous,Frequency frequency = Annual);*/

    //Courbe de vol constante 
    auto volatilityCurve = ext::make_shared<BlackConstantVol>(0, TARGET(), Handle<Quote>(sigma), Actual360());


    /*BlackConstantVol(const Date& referenceDate,const Calendar&, Volatility volatility, const DayCounter& dayCounter);

    BlackConstantVol(const Date& referenceDate,const Calendar&,Handle<Quote> volatility,const DayCounter& dayCounter);

    BlackConstantVol(Natural settlementDays,const Calendar&,Volatility volatility,const DayCounter& dayCounter);

    BlackConstantVol(Natural settlementDays,const Calendar&,Handle<Quote> volatility,const DayCounter& dayCounter);*/
    //Pricing Methods = pricing engines holding the required market data

    /********************************************************************/
    //Process : Equation de l'évolution du sous jacent dans le temps (Parce que S(t) est un stochastique process)
    //Comment les variables évoluent ?
    //BS Process: dS_t = r S_t dt + \sigma S_t dW_t

    auto process = ext::make_shared<BlackScholesProcess>(Handle<Quote>(u), Handle<YieldTermStructure>(riskFreeCurve), Handle<BlackVolTermStructure>(volatilityCurve));
    //YieldTermStructure = objet représentant une courbe de taux selon la maturité
    //BlackVolTermStructure = objet représentant une courbe de vol selon la maturité

    /************************************************************************/
    //model (nécessaire pour les modèles qui ont besoin de calibration des market data, permet entre autre le recalcul automatique après calibration)

    //auto Hestonmodel = ext::make_shared<HestonModel>(const ext::shared_ptr<HestonProcess>& process);


    /***********************************************************************/
    //Pricing engine (l'objet qui sait calculer le prix)
    //AnalyticEuropeanEngine Applique la formule fermée de Black Scholes
    auto engine = ext::make_shared<AnalyticEuropeanEngine>(process);

    /*MCEuropeanEngine(
             const ext::shared_ptr<GeneralizedBlackScholesProcess>& process,
             Size timeSteps, //Nombre de steps sur toute la durée de vie du produit
             Size timeStepsPerYear, //Nombre de teps par an

             bool brownianBridge, 
             bool antitheticVariate, 

             Size requiredSamples, //Nombre de paths à simuler 
             ou 
             Real requiredTolerance, //L'erreur statiistique maximale qu'on ne veut pas dépasser 
             Size maxSamples, //Nombre maximal de samples (paths)

             BigNatural seed);*/
    
    auto mcEngine =MakeMCEuropeanEngine<PseudoRandom>(process)
        .withSteps(20)
        .withAntitheticVariate()
        .withSamples(25000)
        .withSeed(42);
    //auto Hestonengine = ext::make_shared<AnalyticHestonEngine>(Hestonmodel);

    /**************************************************************************/
    //Pricing
    option.setPricingEngine(mcEngine);

    std::cout << option.NPV()<< std::endl; //Lazy calculation

    option.setPricingEngine(engine);

    std::cout << option.NPV()<< std::endl;

    //Greeks
    std::cout << option.delta()<< std::endl; //Pas de greeks pour Mc
    std::cout << option.gamma()<< std::endl;
    std::cout << option.vega()<< std::endl;

    /***************************************************************************/
    //Change in market data
    u->setValue(105.);
    std::cout << option.NPV()<< std::endl;

    Date futur(27, October, 2026);
    Settings::instance().evaluationDate() = futur;
    std::cout << option.NPV()<< std::endl;
    return 0;
}