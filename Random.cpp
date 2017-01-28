
#include "StdAfx.h"
#include "Random.h"


// *******************************
// rand() --
// Returns a pseudorandom value between range 0.0 and 1.0.
// *******************************

float Random::rand()
{
    // THIS IS NOT HOW THE CLIENT DOES IT.
    DWORD RandInt = (unsigned)::rand();
    float Result = ((float)RandInt) / ((float)RAND_MAX);

    return Result;
}

// *******************************
// RollDice()
// Returns a pseudorandom value between ranges A and B.
// *******************************
float Random::RollDice(float RangeA, float RangeB )
{
    if (RangeA == RangeB )
        return RangeA;

    float RangeLow = RangeA;
    float RangeHigh = RangeB;

    // The ranges are reversed
    if (RangeHigh < RangeLow )
    {
        RangeLow = RangeB;
        RangeHigh = RangeA;
    }
 
    float Result = RangeLow + ((RangeHigh - RangeLow) * Random::rand());

    // DEBUGOUT("RollDice(%f, %f ) == %f\r\n", RangeA, RangeB, Result );
    return Result;
}




