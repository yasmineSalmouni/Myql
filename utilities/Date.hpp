#pragma once
#include <ctime>

namespace Myql
{

    class Date
    {
    public:

        Date()
        : day_(1),
        month_(1),
        year_(1980)
        {
        }

        Date(
            int day,
            int month,
            int year)
        : day_(day),
        month_(month),
        year_(year)
        {
        }

        static Date today()
        {
            std::time_t t = std::time(nullptr);
            std::tm* now = std::localtime(&t);
            return Date(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
        }

   
        int day() const
        {
            return day_;
        }

        int month() const
        {
            return month_;
        }

        int year() const
        {
            return year_;
        }

        int daysSince1980() const
        {
            std::tm origin = {};
            origin.tm_mday = 1;
            origin.tm_mon  = 0;      // janvier
            origin.tm_year = 1980 - 1900;

            std::tm current = {};
            current.tm_mday = day_;
            current.tm_mon  = month_ - 1;
            current.tm_year = year_ - 1900;

            std::time_t t0 = std::mktime(&origin);
            std::time_t t1 = std::mktime(&current);

            return static_cast<int>(
                (t1 - t0) / (24 * 60 * 60));
        }

        bool operator==(const Date& other) const
        {
            return day_   == other.day_
                && month_ == other.month_
                && year_  == other.year_;
        }

        bool operator<(const Date& other) const
        {
            if (year_ != other.year_)
                return year_ < other.year_;

            if (month_ != other.month_)
                return month_ < other.month_;

            return day_ < other.day_;
        }

    private:

        int day_;
        int month_;
        int year_;
    };

}


