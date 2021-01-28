#include "Stdafx.h"
#include "Math/Math.h"
//#include <wchar.h>

extern "C" const Double Math_PI = 3.14159265358979324;
extern "C" const Double Math_E = 2.71828182845904523;
const Double Math::PI = Math_PI;

Double Math::Math_Log10(Double val)
{
    return Math::Math_Ln(val) / 2.302585092994;
}

Double Math::Math_Ln(Double val)
{
    if (val < 0)
        return 0;
    Double tmp = 0;
    while (val >= Math_E)
    {
        val /= Math_E;
        tmp++;
    }
    Double total = 0;
    Double divV = 1;
    Double mulV = (val - 1) / (val + 1);
    Double mulV2 = mulV * mulV;
    Double thisVal;
    while (true)
    {
        thisVal = mulV / divV;
        if (total + thisVal == total)
            break;
        total += thisVal;
        mulV *= mulV2;
        divV += 2;
        if (divV > 100)
            break;
    }
    return tmp + total * 2;
}

Double Math::Math_Exp(Double val)
{
    Double total = 1;
    Double lastVal = 1;
    Double thisVal;
    Int32 divBase = 1;
    Bool isNeg = false;
    if (val < 0)
    {
        isNeg = true;
        val = -val;
    }
    while (true)
    {
        thisVal = lastVal * val / divBase;
        if (total + thisVal == total)
            break;
        total += thisVal;
        lastVal = thisVal;
        divBase++;
        if (divBase > 100)
            break;
    }
    if (isNeg)
    {
        return 1 / total;
    }
    else
    {
        return total;
    }
}

Double Math::Math_Pow(Double x, Double y)
{
    return Math_Exp(y * Math_Ln(x));
}
