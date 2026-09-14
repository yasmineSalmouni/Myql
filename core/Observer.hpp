#pragma once


namespace Myql
{
    class Observable; //forward declaration

    class Observer
    {
    public:
        virtual ~Observer() = default; //ecriture moderne de virtual Õbserver(){}

        virtual void update() = 0;

        void registerWith(Observable*);
        void unregisterWith(Observable*);
    };
}
