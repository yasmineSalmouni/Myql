#include "EQA.hpp"
#include "../utilities/Date.hpp"
#include <iostream>
#include <iomanip>

#include <fstream>


namespace Myql
{
    EQA::EQA(Date exerciceDate,Date evaluationDate, std::vector<Date> observationDates,double Nominal,double Coupon,double Bautocall,double Bprotection,double Bcoupon,int M,double T,bool Memory, string FinalPayoff, string KIType, string KIObservationType, bool CustomKIschedual, double K, double alpha)
        : exerciceDate_(exerciceDate),evaluationDate_(evaluationDate), observationDates_(observationDates),
        Nominal_(Nominal),Coupon_(Coupon),Bautocall_(Bautocall),Bprotection_(Bprotection),Bcoupon_(Bcoupon),
        M_(M),T_(T), Memory_(Memory), FinalPayoff_(FinalPayoff), KIType_(KIType),  KIObservationType_(KIObservationType), CustomKIschedual_(CustomKIschedual), K_(K),  alpha_(alpha) {}

    bool EQA::isExpired() const
    {
        if(exerciceDate_ < evaluationDate_)
        {
            return true;
        }
        return false;
    }

    void EQA::setupArguments(PricingEngine::arguments* a) const
    {
        EQA::arguments* args = dynamic_cast<EQA::arguments*>(a);
        MYQL_REQUIRE(args, "wrong engine type");
        args->exerciceDate = exerciceDate_;
        args->evaluationDate = evaluationDate_;
        args->observationDates = observationDates_;
        args->Nominal = Nominal_;
        args->Coupon = Coupon_;
        args->Bautocall = Bautocall_;
        args->Bprotection = Bprotection_;
        args->Bcoupon = Bcoupon_;
        args->M = M_;
        args->T = T_;
        args->Memory = Memory_;
        args->FinalPayoff = FinalPayoff_;
        args->KIType = KIType_;
        args->KIObservationType = KIObservationType_;
        args->CustomKIschedual = CustomKIschedual_;
        args->K = K_;
        args->alpha = alpha_;
    }

    std::vector<double> EQA::buildDeltaT(const Date& evaluationDate,const std::vector<Date>& observationDates)
    {
        /*std::ofstream logFile3("Dt.txt");

        if (!logFile3.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier\n";
        }
        std::streambuf* oldCoutBuffer3 = std::cout.rdbuf();
        std::cout.rdbuf(logFile3.rdbuf());**/
        std::vector<double> deltaT;

        int prevDate = evaluationDate.daysSince1980();
        int i =0;

        for (const auto& date : observationDates)
        {
            int currentDate = date.daysSince1980();

            double dt =(currentDate - prevDate) / 365.0;

            //std::cout << "dt[" << i << "]"<< dt <<std::endl;

            deltaT.push_back(dt);

            prevDate = currentDate;

            i++;
        }
         //std::cout.rdbuf(oldCoutBuffer3);

        return deltaT;
    }

}